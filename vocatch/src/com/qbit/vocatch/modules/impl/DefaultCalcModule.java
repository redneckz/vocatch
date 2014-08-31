package com.qbit.vocatch.modules.impl;

import com.qbit.vocatch.env.Env;

import com.qbit.vocatch.modules.CalcModule;
import com.qbit.vocatch.modules.Slide;

public final class DefaultCalcModule implements CalcModule {
	
	static {
		System.loadLibrary("def-angle-calc-module");
	}

	private Env env;
	
	@Override
	public void init(Env env) {
		this.env = env;
	}

	@Override
	public String getId() {
		return "calc.default";
	}

	@Override
	public String getName() {
		return "Стандартный CALC-модуль";
	}

	@Override
	public void calc(Slide slide) throws Throwable {
		if (!(slide instanceof DefaultSlide)) {
			throw new Exception("Invalid slide format!");
		}
		DefaultSlide mySlide = (DefaultSlide) slide;
		work(mySlide,
				env.getModuleShortProperty(getId(), "filter.radius"),
				env.getModuleByteProperty(getId(), "threshold.rigidity"),
				env.getModuleByteProperty(getId(), "filter.debug.level"));
	}

	private native void work(DefaultSlide mySlide, short filterRadius, byte thRigidity, byte filterDebugLevel);

}
