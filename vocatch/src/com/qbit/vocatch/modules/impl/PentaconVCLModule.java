package com.qbit.vocatch.modules.impl;

import com.qbit.vocatch.env.Env;
import com.qbit.vocatch.main.Constants;
import com.qbit.vocatch.modules.SlideBuffer;
import com.qbit.vocatch.modules.VCLModule;

public final class PentaconVCLModule implements VCLModule {

	static {
		if (!Constants.DEBUG) {
			System.loadLibrary("pentacon-vcl-module");
		}
	}

	private Env env;
	
	@Override
	public void init(Env env) {
		this.env = env;
	}

	@Override
	public String getId() {
		return "vcl.pentacon";
	}

	@Override
	public String getName() {
		return "Pentacon VCL-модуль";
	}

	@Override
	public SlideBuffer createBuffer(int slideCount) {
		return new DefaultSlideBuffer(slideCount, env.getModuleIntProperty(getId(), "slide.width"));
	}

	@Override
	public long captureVideo(SlideBuffer buffer, int offset, int count)
			throws Throwable {
		if (!(buffer instanceof DefaultSlideBuffer)) {
			throw new Exception("Unsupported buffer format!");
		}
		DefaultSlideBuffer myBuffer = (DefaultSlideBuffer) buffer;
		long startTime = work(myBuffer, offset, count, env.getModuleByteProperty(getId(), "video.format"));
		if (startTime < 0) {
			throw new Exception("Capture failed!");
		}
		return startTime;
	}

	private native long work(DefaultSlideBuffer mySlide, int offset, int count, byte vfIndex);

}
