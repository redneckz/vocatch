package com.qbit.vocatch.env;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;
import com.qbit.vocatch.main.PersistanceConstants;
import com.qbit.vocatch.modules.CalcModule;
import com.qbit.vocatch.modules.VCLModule;

/**
 *
 * @author Александр
 */
public final class Env {
	
	private final Properties properties = new Properties();
	
	public Env() {
	}
	
	public synchronized int getBufferSize() {
		String bufferSizeStr = properties.getProperty("vocatch.mode.buffer.size");
		if (bufferSizeStr != null) {
			return Integer.parseInt(bufferSizeStr);
		} else {
			return 1;
		}
	}
	
	public synchronized File getWorkingDirectory() {
		return new File(properties.getProperty("vocatch.mode.working.directory", "." + File.separator));
	}
	
	public synchronized Class<CalcModule> getCalcModuleClass() {
		try {
			return (Class<CalcModule>) Class.forName(properties.getProperty("vocatch.module.calc"));
		} catch (Exception ex) {
			Logger.getLogger(PersistanceConstants.LOGGER__NAME).log(Level.SEVERE, null, ex);
			return null;
		}
	}
	
	public synchronized Class<VCLModule> getVCLModuleClass() {
		try {
			return (Class<VCLModule>) Class.forName(properties.getProperty("vocatch.module.vcl"));
		} catch (Exception ex) {
			Logger.getLogger(PersistanceConstants.LOGGER__NAME).log(Level.SEVERE, null, ex);
			return null;
		}
	}
	
	public synchronized String getModuleProperty(String moduleId, String propName) {
		return getModuleProperty(moduleId, propName, null);
	}
	
	public synchronized String getModuleProperty(String moduleId, String propName, String defaulVal) {
		return properties.getProperty("vocatch.module." + moduleId.toLowerCase() + "." + propName, defaulVal);
	}
	
	public synchronized int getModuleIntProperty(String moduleId, String propName) {
		String valStr = getModuleProperty(moduleId, propName, "0");
		return Integer.parseInt(valStr);
	}
	
	public synchronized short getModuleShortProperty(String moduleId, String propName) {
		String valStr = getModuleProperty(moduleId, propName, "0");
		return Short.parseShort(valStr);
	}
	
	public synchronized byte getModuleByteProperty(String moduleId, String propName) {
		String valStr = getModuleProperty(moduleId, propName, "0");
		return Byte.parseByte(valStr);
	}
	
	public synchronized double getModuleDoubleProperty(String moduleId, String propName) {
		String valStr = getModuleProperty(moduleId, propName, "0");
		return Double.parseDouble(valStr);
	}
	
	public synchronized void load() {
		InputStream input = null;
		try {
			input = new FileInputStream(PersistanceConstants.ENV__FILE_PATH);
			properties.load(input);
		} catch (Exception ex) {
			Logger.getLogger(PersistanceConstants.LOGGER__NAME).log(Level.SEVERE, ex.getMessage(), ex);
		} finally {
			try {
				if (input != null) {
					input.close();
				}
			} catch (Exception ex) {
				Logger.getLogger(PersistanceConstants.LOGGER__NAME).log(Level.SEVERE, ex.getMessage(), ex);
			}
		}
	}
}
