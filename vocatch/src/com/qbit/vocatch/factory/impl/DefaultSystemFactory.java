package com.qbit.vocatch.factory.impl;

import com.qbit.vocatch.env.Env;
import com.qbit.vocatch.env.EnvReloader;
import java.util.logging.Logger;


import com.qbit.vocatch.factory.SystemFactory;
import com.qbit.vocatch.main.PersistanceConstants;
import static com.qbit.vocatch.main.PersistanceConstants.ERROR_LOG__FILE_PATH;
import static com.qbit.vocatch.main.PersistanceConstants.INFO_LOG__FILE_PATH;
import com.qbit.vocatch.modules.CalcModule;
import com.qbit.vocatch.modules.VCLModule;
import com.qbit.vocatch.proxy.ComCenterProxy;
import com.qbit.vocatch.proxy.CommandListener;
import com.qbit.vocatch.proxy.impl.UDPComCenterProxy;
import com.qbit.vocatch.worker.Worker;
import com.qbit.vocatch.worker.impl.WorkerImpl;
import java.util.concurrent.Executors;
import java.util.logging.FileHandler;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.SimpleFormatter;

public final class DefaultSystemFactory implements SystemFactory {
	
	private static class DefaultPartsFactoryHolder {

		private static final DefaultSystemFactory INSTANCE = new DefaultSystemFactory();
	}

	public static SystemFactory inst() {
		return DefaultPartsFactoryHolder.INSTANCE;
	}
	
	private Env env;

	private ComCenterProxy comCenterProxy;

	private Worker worker;
	
	private DefaultSystemFactory() {
	}
	
	@Override
	public Logger configureLogger() {
		try {
			Logger logger = Logger.getLogger(PersistanceConstants.LOGGER__NAME);
			Handler infoHandler = new FileHandler(INFO_LOG__FILE_PATH);
			infoHandler.setLevel(Level.INFO);
			infoHandler.setEncoding("UTF-8");
			infoHandler.setFormatter(new SimpleFormatter());
			logger.addHandler(infoHandler);
			Handler errorHandler = new FileHandler(ERROR_LOG__FILE_PATH);
			errorHandler.setLevel(Level.SEVERE);
			errorHandler.setEncoding("UTF-8");
			errorHandler.setFormatter(new SimpleFormatter());
			logger.addHandler(errorHandler);
			logger.setLevel(Level.ALL);
			return logger;
		} catch (Exception ex) {
			throw new Error(ex);
		}
	}
	
	@Override
	public synchronized Env createEnv() {
		if (env == null) {
			env = new Env();
			env.load();
			EnvReloader envReloader = new EnvReloader(env);
			Executors.newSingleThreadExecutor().execute(envReloader);
		}
		return env;
	}
	
	@Override
	public synchronized ComCenterProxy createComCenterProxy(CommandListener l) {
		if (comCenterProxy == null) {
			try {
				comCenterProxy = new UDPComCenterProxy(createWorker());
				Executors.newSingleThreadExecutor().execute(comCenterProxy);
			} catch (Exception e) {
				Logger.getLogger(PersistanceConstants.LOGGER__NAME).severe(e.getMessage());
			}
		}
		comCenterProxy.addCommandListener(l);
		return comCenterProxy;
	}
	
	@Override
	public synchronized CalcModule createCalcModule() {
		try {
			CalcModule calcModule = createEnv().getCalcModuleClass().newInstance();
			calcModule.init(createEnv());
			return calcModule;
		} catch (Exception ex) {
			Logger.getLogger(PersistanceConstants.LOGGER__NAME).log(Level.SEVERE, ex.getMessage(), ex);
			return null;
		}
	}
	
	@Override
	public synchronized VCLModule createVCLModule() {
		try {
			VCLModule vclModule = createEnv().getVCLModuleClass().newInstance();
			vclModule.init(createEnv());
			return vclModule;
		} catch (Exception ex) {
			Logger.getLogger(PersistanceConstants.LOGGER__NAME).log(Level.SEVERE, ex.getMessage(), ex);
			return null;
		}
	}

	@Override
	public synchronized Worker createWorker() {
		if (worker == null) {
			worker = new WorkerImpl(createEnv(), createCalcModule(), createVCLModule());
		}
		return worker;
	}

}
