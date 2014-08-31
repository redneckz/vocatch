package com.qbit.vocatch.worker;

import com.qbit.vocatch.modules.SlideBuffer;

import org.tis.utils.design.mvc.PropertyChangeNotifier;

public interface Worker extends SlideBuffer, PropertyChangeNotifier {

	public static final String PNAME__WORK_STARTED = "workStarted";

	public static final String PNAME__WAITING_SC_CHANGED = "waitingSC";

	public static final String PNAME__NEXT_SLIDE_READY = "slideReady";

	public static final String PNAME__WORK_ENDED = "workEnded";
	
	public boolean isWorking();

	public void prepare() throws WorkerException;

	public boolean work() throws WorkerException;

	public void stop();

}
