package com.qbit.vocatch.worker.impl;

import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.text.DateFormat;
import java.util.Date;

/**
 *
 * @author Александр
 */
public final class VOCDataIO {
	
	private VOCDataIO() {
	}
	
	public static VOCDataBuffer createDataBuffer() {
		return new VOCDataBuffer();
	}

	public static File saveToFile(VOCDataBuffer dataBuffer, File workingDirectory) throws Exception {
		File file = prepareCurrentVOCDataFile(workingDirectory);
		OutputStream out = new FileOutputStream(file);
		dataBuffer.writeTo(out);
		return file;
	}

	private static File prepareCurrentVOCDataFile(File workingDirectory) {
		String timestamp = DateFormat.getDateTimeInstance(DateFormat.SHORT, DateFormat.SHORT).format(new Date());
		File currentVOCDataFile = new File(workingDirectory, timestamp + ".pop");
		if (currentVOCDataFile.exists()) {
			currentVOCDataFile.delete();
		}
		return currentVOCDataFile;
	}
}
