package com.qbit.vocatch.modules.impl;

import com.qbit.vocatch.modules.SlideBuffer;

/**
 * Выполнено как структура из соображений производительности и удоства доступа из JNI
 * 
 * @author Александр
 */

final class DefaultSlideBuffer implements SlideBuffer {

	final int slideCount;

	final int slideWidth;

	final byte[] pixels;

	final DefaultSlide[] slides;

	DefaultSlideBuffer(int slideCount, int slideWidth) {
		this.slideCount = slideCount;
		this.slideWidth = slideWidth;
		pixels = new byte[slideCount * slideWidth * slideWidth];
		slides = new DefaultSlide[slideCount];
	}

	@Override
	public int getSlideCount() {
		return slideCount;
	}

	@Override
	public int getSlideWidth() {
		return slideWidth;
	}

	@Override
	public synchronized DefaultSlide getSlideByIndex(int slideIndex) {
		DefaultSlide slide = slides[slideIndex];
		if (slide == null) {
			slide = new DefaultSlide(this, slideIndex);
			slides[slideIndex] = slide;
		}
		return slide;
	}
}
