package com.qbit.vocatch.worker;

public class WorkerException extends Exception {

	private static final long serialVersionUID = 7372795330734493821L;

	public WorkerException(final Throwable cause) {
		super(cause.getMessage(), cause);
	}

}
