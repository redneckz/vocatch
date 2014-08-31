package com.qbit.vocatch.worker.impl;

import com.qbit.vocatch.main.PersistanceConstants;
import com.qbit.vocatch.modules.ObjectParams;
import com.qbit.vocatch.modules.Slide;
import com.qbit.vocatch.modules.SlideIO;
import java.io.ByteArrayOutputStream;
import java.io.Closeable;
import java.io.IOException;
import java.io.OutputStream;
import java.util.logging.Logger;

/**
 *
 * @author Александр
 */
public final class VOCDataBuffer implements Closeable {

	private static final int VOC_DATA_BUFFER_SIZE = 4096 * 32;
	
	private static final int TIME_LINE_PART_SIZE = 8;
	
	private static final int RESULT_PART_SIZE = 4;

	private ByteArrayOutputStream timeLineBuffer = new ByteArrayOutputStream(VOC_DATA_BUFFER_SIZE);

	private ByteArrayOutputStream resultBuffer = new ByteArrayOutputStream(VOC_DATA_BUFFER_SIZE);

	VOCDataBuffer() {
	}
	
	public void reset() {
		timeLineBuffer.reset();
		resultBuffer.reset();
	}

	public void saveSlideParams(Slide slide) {
		try {
			saveTwoBytePart(resultBuffer, getAngleInMinutes(slide.getObjectParams()));
			saveTwoBytePart(resultBuffer, SlideIO.getObjectOffset(slide));
		} catch (IOException ex) {
			Logger.getLogger(PersistanceConstants.LOGGER__NAME).severe(ex.getMessage());
		}
	}
	
	public void saveCaptureTime(long captureTime) {
		try {
			saveOctBytePart(timeLineBuffer, captureTime);
		} catch (IOException ex) {
			Logger.getLogger(PersistanceConstants.LOGGER__NAME).severe(ex.getMessage());
		}
	}
	
	public void writeTo(OutputStream out) throws IOException {
		byte[] timeLineArray = timeLineBuffer.toByteArray();
		byte[] resultArray = resultBuffer.toByteArray();
		int slideCount = timeLineBuffer.size() / TIME_LINE_PART_SIZE;
		for (int slideIndex = 0; slideIndex < slideCount; slideIndex++) {
			out.write(timeLineArray, slideIndex * TIME_LINE_PART_SIZE, TIME_LINE_PART_SIZE);
			out.write(resultArray, slideIndex * RESULT_PART_SIZE, RESULT_PART_SIZE);
		}
	}

	@Override
	public void close() throws IOException {
		reset();
		timeLineBuffer = null;
		resultBuffer = null;
	}

	private static int getAngleInMinutes(ObjectParams currentBodyParams) {
		return (int) (currentBodyParams.getAngle() * 180.0 * 60.0 / Math.PI);
	}

	private static void saveOctBytePart(OutputStream out, long part) throws IOException {
		saveTwoBytePart(out, (int) (part >>> 48));
		saveTwoBytePart(out, (int) (part >>> 32));
		saveTwoBytePart(out, (int) (part >>> 16));
		saveTwoBytePart(out, (int) part);
	}

	private static void saveTwoBytePart(OutputStream out, int part) throws IOException {
		out.write(part >>> 8);
		out.write(part);
	}
}
