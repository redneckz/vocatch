package com.qbit.vocatch.modules;

public interface VCLModule extends Module {
	
	public SlideBuffer createBuffer(int slideCount);

	public long captureVideo(SlideBuffer buffer, int offset, int count) throws Throwable;

}
