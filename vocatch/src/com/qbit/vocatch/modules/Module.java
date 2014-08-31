package com.qbit.vocatch.modules;

import com.qbit.vocatch.env.Env;

public interface Module {
	
	public void init(Env env);
	
	public String getId();

	public String getName();

}
