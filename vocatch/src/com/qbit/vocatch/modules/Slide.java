package com.qbit.vocatch.modules;

public interface Slide {
	
	public SlideBuffer getParent();
	
	public int getSlideWidth();

	public ObjectParams getObjectParams();
	
	public byte[] getRatser();

}
