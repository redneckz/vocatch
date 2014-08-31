package com.qbit.vocatch.modules.impl;

import com.qbit.vocatch.modules.ObjectParams;
import com.qbit.vocatch.modules.Slide;
import com.qbit.vocatch.modules.SlideBuffer;
import java.util.Arrays;

/**
 * Выполнено как структура из соображений производительности и удоства доступа из JNI.
 * Вычислимые свойства кэшируются в финальные поля заранее.
 * 
 * @author Александр
 */
final class DefaultSlide implements Slide, ObjectParams {

	final DefaultSlideBuffer parent;

	final int offset;

	final int size;
	
	volatile double angle;
	
	volatile int x;
	
	volatile int y;

	DefaultSlide(DefaultSlideBuffer parent, int slideIndex) {
		this.parent = parent;
		size = parent.slideWidth * parent.slideWidth;
		offset = slideIndex * size;
	}
	
	@Override
	public SlideBuffer getParent() {
		return parent;
	}

	@Override
	public int getSlideWidth() {
		return parent.slideWidth;
	}

	@Override
	public ObjectParams getObjectParams() {
		return this;
	}

	@Override
	public byte[] getRatser() {
		return Arrays.copyOfRange(parent.pixels, offset, offset + size);
	}

	@Override
	public double getAngle() {
		return angle;
	}

	@Override
	public int getX() {
		return x;
	}

	@Override
	public int getY() {
		return y;
	}
}
