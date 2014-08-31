package com.qbit.vocatch.proxy;

public interface ComCenterProxy extends Runnable {

	public void addCommandListener(CommandListener l);

	public void removeCommandListener(CommandListener l);

}
