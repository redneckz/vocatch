package com.qbit.vocatch.main;

import java.io.File;

public interface PersistanceConstants {

	public static final String ENV__FOLDER = "config" + File.separator;
	
	public static final String ENV__FILE_PATH = ENV__FOLDER + "vocatch.properties";

	public static final String LOG__FOLDER_PATH = "log" + File.separator;

	public static final String ERROR_LOG__FILE_PATH = LOG__FOLDER_PATH + "error-log.txt";

	public static final String INFO_LOG__FILE_PATH = LOG__FOLDER_PATH + "info-log.txt";

	public static final String LOGGER__NAME = "vocatch";

}
