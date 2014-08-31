package com.qbit.vocatch.modules.impl;

import com.qbit.vocatch.env.Env;
import static org.tis.utils.io.file.FileFilterFactory.AND;
import static org.tis.utils.io.file.FileFilterFactory.OR;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileFilter;

import javax.imageio.ImageIO;

import org.tis.utils.io.file.FileUtils;

import com.qbit.vocatch.modules.SlideBuffer;
import com.qbit.vocatch.modules.VCLModule;

public final class DemoVCLModule implements VCLModule {

	private static final class IsReadableFileFilter implements FileFilter {

		@Override
		public boolean accept(File file) {
			return file.isFile() && file.canRead();
		}

	};

	private static final class ByExtFileFilter implements FileFilter {

		private final String ext;

		public ByExtFileFilter(String ext) {
			this.ext = ext;
		}

		@Override
		public boolean accept(File file) {
			return this.ext.equals(FileUtils.getExtension(file));
		}

	};

	private static final FileFilter FILTER = AND(new IsReadableFileFilter(),
			OR(new ByExtFileFilter("jpeg"), OR(new ByExtFileFilter("jpg"),
					OR(new ByExtFileFilter("gif"),
							OR(new ByExtFileFilter("bmp"),
									new ByExtFileFilter("png"))))));
	
	private Env env;
	
	@Override
	public void init(Env env) {
		this.env = env;
	}

	@Override
	public String getId() {
		return "vcl.demo";
	}

	@Override
	public String getName() {
		return "Демонстрационный VCL-модуль";
	}

	@Override
	public SlideBuffer createBuffer(int slideCount) {
		return new DefaultSlideBuffer(slideCount, env.getModuleIntProperty(getId(), "slide.width"));
	}

	private static int fileIndex = 0;

	@Override
	public long captureVideo(SlideBuffer buffer, int offset, int count)
			throws Throwable {
		if (!(buffer instanceof DefaultSlideBuffer)) {
			throw new Exception("Unsupported buffer format!");
		}
		DefaultSlideBuffer myBuffer = (DefaultSlideBuffer) buffer;
		String slidesFolderPath = env.getModuleProperty(getId(), "sildes.folder");
		File slidesFolder = new File(slidesFolderPath);
		if (!slidesFolder.isDirectory()) {
			throw new Exception("Invalid slides folder!");
		}
		byte[] raster = new byte[myBuffer.slideWidth * myBuffer.slideWidth];
		long startTime = System.currentTimeMillis();
		File[] files = slidesFolder.listFiles(FILTER);
		for (int slideIndex = offset; fileIndex < files.length && count > 0 && slideIndex < buffer.getSlideCount();
				fileIndex = (fileIndex + 1) % files.length, count--, slideIndex++) {
			BufferedImage loadedImage = ImageIO.read(files[fileIndex]);
			int w = loadedImage.getWidth();
			int h = loadedImage.getHeight();
			int currentL = Math.min(w, h);
			if (currentL < myBuffer.slideWidth) {
				continue;
			}
			BufferedImage bufferedImage = new BufferedImage(w, h,
					BufferedImage.TYPE_BYTE_GRAY);
			bufferedImage.setRGB(0, 0, w, h, loadedImage.getRGB(0, 0, w, h,
					null, 0, w), 0, w);
			copyImageInSlideBuffer(bufferedImage, myBuffer, slideIndex, raster);
		}
		return startTime;
	}

	private void copyImageInSlideBuffer(BufferedImage bufferedImage,
			DefaultSlideBuffer myBuffer, int slideIndex, byte[] raster) {
		DefaultSlide mySlide = myBuffer.getSlideByIndex(slideIndex);
		int w = bufferedImage.getWidth();
		int h = bufferedImage.getHeight();
		int startX = (w - myBuffer.slideWidth) / 2;
		int startY = (h - myBuffer.slideWidth) / 2;
		bufferedImage.getData().getDataElements(startX, startY, myBuffer.slideWidth,
				myBuffer.slideWidth, raster);
		System.arraycopy(raster, 0, myBuffer.pixels, mySlide.offset, mySlide.size);
	}

}
