package com.qbit.vocatch.factory;

import com.qbit.vocatch.env.Env;
import com.qbit.vocatch.modules.CalcModule;
import com.qbit.vocatch.modules.VCLModule;
import com.qbit.vocatch.proxy.ComCenterProxy;
import com.qbit.vocatch.proxy.CommandListener;
import com.qbit.vocatch.worker.Worker;
import java.util.logging.Logger;

public interface SystemFactory {
	
	public Logger configureLogger();
	
	public Env createEnv();

	public ComCenterProxy createComCenterProxy(CommandListener l);
	
	public CalcModule createCalcModule();
	
	public VCLModule createVCLModule();

	public Worker createWorker();

}
