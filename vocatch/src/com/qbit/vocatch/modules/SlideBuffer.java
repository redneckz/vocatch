package com.qbit.vocatch.modules;

public interface SlideBuffer {

	public int getSlideWidth();

	public int getSlideCount();

	public Slide getSlideByIndex(int slideIndex);

}
