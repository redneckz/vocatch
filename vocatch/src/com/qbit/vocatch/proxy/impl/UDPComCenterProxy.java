package com.qbit.vocatch.proxy.impl;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.util.HashSet;
import java.util.Set;
import java.util.logging.Logger;

import com.qbit.vocatch.main.PersistanceConstants;
import com.qbit.vocatch.proxy.ComCenterProxy;
import com.qbit.vocatch.proxy.Command;
import com.qbit.vocatch.proxy.CommandListener;
import com.qbit.vocatch.worker.Worker;
import com.qbit.vocatch.worker.WorkerException;
import java.io.IOException;
import java.net.SocketAddress;

public final class UDPComCenterProxy implements ComCenterProxy {
	
	private static final int PORT = 3333;
	
	private static final byte OK_CODE = 0x55; 
	
	private static final byte[] OK_PACKET = new byte[] {OK_CODE};
	
	private static final int PACKET_SIZE_FIELD_SIZE = 2;
	private static final int DATA_PACKET_SIZE = PACKET_SIZE_FIELD_SIZE + 4096;

	private final DatagramSocket socket;

	private final byte[] commandBuf = new byte[] {-1};

	private final byte[] dataBuf = new byte[DATA_PACKET_SIZE];

	private final Worker worker;

	private volatile boolean workEndedFlag;

	private volatile File currentVOCDataFile;
	
	private final Set<CommandListener> listeners = new HashSet<>();

	public UDPComCenterProxy(Worker worker) throws Exception {
		socket = new DatagramSocket(PORT);
		socket.setBroadcast(true);
		this.worker = worker;
		worker.addPropertyChangeListener(Worker.PNAME__WORK_ENDED,
				new PropertyChangeListener() {

					@Override
					public void propertyChange(PropertyChangeEvent evt) {
						synchronized (UDPComCenterProxy.this) {
							currentVOCDataFile = (File) evt.getNewValue();
							workEndedFlag = true;
							UDPComCenterProxy.this.notify();
						}
					}

				});
	}

	@Override
	public void run() {
		DatagramPacket commandPacket = new DatagramPacket(commandBuf, commandBuf.length);
		while (true) {
			try {
				socket.receive(commandPacket);
				int commandType = commandBuf[0];
				if ((commandType < 0) || (commandType >= Command.values().length)) {
					continue;
				}
				Command cmd = Command.values()[commandType];
				fireCommand(cmd);
				switch (cmd) {
					case PREPARE:
						prepare(commandPacket.getSocketAddress());
						break;
					case START_CYCLE:
						startCycle();
						break;
					case STOP_CYCLE:
						stopCycle(commandPacket.getSocketAddress());
						break;
				}
			} catch (Exception e) {
				Logger.getLogger(PersistanceConstants.LOGGER__NAME).severe(e.getMessage());
			}
		}
	}
	
	private void prepare(SocketAddress socketAddress) throws IOException, WorkerException {
		worker.prepare();
		socket.send(new DatagramPacket(OK_PACKET, OK_PACKET.length, socketAddress));
	}
	
	private void startCycle() throws WorkerException {
		synchronized (this) {
			currentVOCDataFile = null;
			workEndedFlag = false;
		}
		worker.work();
	}
	
	private void stopCycle(SocketAddress socketAddress) throws Exception {
		InputStream vocDataInput = null;
		try {
			synchronized (this) {
				worker.stop();
				while (!workEndedFlag) {
					wait();
				}
				if (currentVOCDataFile != null) {
					vocDataInput = new FileInputStream(currentVOCDataFile);
					int packetLen;
					while ((packetLen = vocDataInput.read(dataBuf, PACKET_SIZE_FIELD_SIZE, dataBuf.length - PACKET_SIZE_FIELD_SIZE)) > 0) {
						sendData(socketAddress, packetLen);
					}
				}
			}
		} finally {
			if (vocDataInput != null) {
				try {
					vocDataInput.close();
				} catch (IOException ex) {
					Logger.getLogger(PersistanceConstants.LOGGER__NAME).severe(ex.getMessage());
				}
			}
			sendData(socketAddress, 0);
		}
	}
	
	private void sendData(SocketAddress socketAddress, int len) {
		dataBuf[0] = (byte) (len >> 8);
		dataBuf[1] = (byte) (len);
		try {
			DatagramPacket vocDataPacket = new DatagramPacket(dataBuf, 0, socketAddress);
			vocDataPacket.setLength(len + PACKET_SIZE_FIELD_SIZE);
			socket.send(vocDataPacket);
		} catch (IOException ex) {
			Logger.getLogger(PersistanceConstants.LOGGER__NAME).severe(ex.getMessage());
		}
	}

	@Override
	public synchronized void addCommandListener(CommandListener listener) {
		listeners.add(listener);
	}

	@Override
	public synchronized void removeCommandListener(CommandListener listener) {
		listeners.remove(listener);
	}

	private synchronized void fireCommand(Command cmd) {
		for (CommandListener listener : listeners) {
			listener.onCommand(cmd);
		}
	}

}
