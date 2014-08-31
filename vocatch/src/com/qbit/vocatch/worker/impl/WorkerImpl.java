package com.qbit.vocatch.worker.impl;

import com.qbit.vocatch.env.Env;
import java.io.File;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.logging.Logger;


import org.tis.utils.design.mvc.impl.PropertyChangeNotifierSupport;

import com.qbit.vocatch.main.PersistanceConstants;
import com.qbit.vocatch.modules.CalcModule;
import com.qbit.vocatch.modules.Slide;
import com.qbit.vocatch.modules.SlideBuffer;
import com.qbit.vocatch.modules.VCLModule;
import com.qbit.vocatch.worker.Worker;
import com.qbit.vocatch.worker.WorkerException;
import java.util.logging.Level;

public class WorkerImpl extends PropertyChangeNotifierSupport implements Worker {

	private final Env env;
	
	private volatile VCLModule vclModule;

	private volatile CalcModule calcModule;
	
	private volatile SlideBuffer slideBuffer;

	private volatile boolean workFlag = false;

	private volatile boolean stopFlag = false;

	private final Lock lock0 = new ReentrantLock(true);

	private final Lock lock1 = new ReentrantLock(true);

	private final Condition notEmptyBuffer = lock1.newCondition();

	private final Condition notFullBuffer = lock1.newCondition();

	private final ExecutorService calcExecutor = Executors.newSingleThreadExecutor();

	private final ExecutorService videoCaptureExecutor = Executors.newSingleThreadExecutor();

	private volatile int waitingSlideCount = 0;

	private final VOCDataBuffer dataBuffer = VOCDataIO.createDataBuffer();

	public WorkerImpl(Env env, CalcModule calcModule, VCLModule vclModule) {
		this.env = env;
		this.calcModule = calcModule;
		this.vclModule = vclModule;
	}

	@Override
	public int getSlideWidth() {
		return slideBuffer != null ? slideBuffer.getSlideWidth() : 0;
	}

	@Override
	public int getSlideCount() {
		return slideBuffer != null ? slideBuffer.getSlideCount() : 0;
	}

	@Override
	public Slide getSlideByIndex(int slideIndex) {
		return slideBuffer != null ? slideBuffer.getSlideByIndex(slideIndex) : null;
	}

	@Override
	public void prepare() throws WorkerException {
		lock0.lock();
		try {
			if (workFlag) {
				return;
			}
			int targetBufferSize = env.getBufferSize();
			if (slideBuffer == null || slideBuffer.getSlideCount() < targetBufferSize) {
				slideBuffer = vclModule.createBuffer(targetBufferSize);
			}
		} catch (Throwable ex) {
			throw new WorkerException(ex);
		} finally {
			lock0.unlock();
		}
	}

	@Override
	public boolean isWorking() {
		lock0.lock();
		try {
			return workFlag;
		} finally {
			lock0.unlock();
		}
	}

	@SuppressWarnings("unchecked")
	@Override
	public boolean work() throws WorkerException {
		lock0.lock();
		try {
			if (workFlag) {
				return false;
			}
			prepare();
			videoCaptureExecutor.submit(videoCaptureTask);
			workFlag = true;
			return true;
		} catch (Throwable ex) {
			throw new WorkerException(ex);
		} finally {
			lock0.unlock();
		}
	}

	@Override
	public void stop() {
		lock0.lock();
		lock1.lock();
		try {
			if (!workFlag) {
				return;
			}
			stopFlag = true;
		} finally {
			lock1.unlock();
			lock0.unlock();
		}
	}

	private final Runnable calcTask = new Runnable() {

		@Override
		public void run() {
			final int bufferSlideCount = slideBuffer.getSlideCount();
			int calculatedSlideCount = 0;
			while (true) {
				lock1.lock();
				try {
					if (stopFlag && waitingSlideCount <= 0) {
						break;
					}
					while (waitingSlideCount <= 0) {
						notEmptyBuffer.await();
					}
				} catch (Throwable e) {
					continue;
				} finally {
					lock1.unlock();
				}
				try {
					Slide slide = slideBuffer.getSlideByIndex(calculatedSlideCount);
					calcModule.calc(slide);
					firePropertyChange(PNAME__NEXT_SLIDE_READY, -1, calculatedSlideCount);
					dataBuffer.saveSlideParams(slide);
				} catch (Throwable ex) {
					Logger.getLogger(PersistanceConstants.LOGGER__NAME).log(Level.SEVERE, ex.getMessage(), ex);
				} finally {
					calculatedSlideCount = (calculatedSlideCount + 1) % bufferSlideCount;
				}
				lock1.lock();
				try {
					waitingSlideCount--;
					notFullBuffer.signal();
				} finally {
					lock1.unlock();
				}
				firePropertyChange(PNAME__WAITING_SC_CHANGED, bufferSlideCount, waitingSlideCount);
			}
		}

	};

	private final Runnable videoCaptureTask = new Runnable() {

		@Override
		public void run() {
			firePropertyChange(PNAME__WORK_STARTED, false, true);
			waitingSlideCount = 0;
			dataBuffer.reset();
			final int bufferSlideCount = slideBuffer.getSlideCount();
			calcExecutor.submit(calcTask);
			int capturedSlideCount = 0;
			while (true) {
				lock1.lock();
				try {
					if (stopFlag) {
						break;
					}
					while (waitingSlideCount >= bufferSlideCount) {
						notFullBuffer.await();
					}
				} catch (Throwable e) {
					continue;
				} finally {
					lock1.unlock();
				}
				try {
					long captureTime = vclModule.captureVideo(slideBuffer, capturedSlideCount, 1);
					dataBuffer.saveCaptureTime(captureTime);
				} catch (Throwable ex) {
					Logger.getLogger(PersistanceConstants.LOGGER__NAME).log(Level.SEVERE, ex.getMessage(), ex);
				} finally {
					capturedSlideCount = (capturedSlideCount + 1) % bufferSlideCount;
				}
				lock1.lock();
				try {
					waitingSlideCount++;
					notEmptyBuffer.signal();
				} finally {
					lock1.unlock();
				}
				firePropertyChange(PNAME__WAITING_SC_CHANGED, bufferSlideCount, waitingSlideCount);
			}
			lock1.lock();
			try {
				while (waitingSlideCount > 0) {
					notFullBuffer.await();
				}
			} catch (Throwable e) {
			} finally {
				lock1.unlock();
			}
			File currentVOCDataFile = null;
			try {
				currentVOCDataFile = VOCDataIO.saveToFile(dataBuffer, env.getWorkingDirectory());
			} catch (Throwable ex) {
				Logger.getLogger(PersistanceConstants.LOGGER__NAME).log(Level.SEVERE, ex.getMessage(), ex);
			}
			lock0.lock();
			lock1.lock();
			try {
				workFlag = false;
				stopFlag = false;
			} finally {
				lock1.unlock();
				lock0.unlock();
			}
			firePropertyChange(PNAME__WORK_ENDED, new Object(), currentVOCDataFile);
		}

	};
}
