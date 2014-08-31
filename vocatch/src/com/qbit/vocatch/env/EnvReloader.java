package com.qbit.vocatch.env;

import com.qbit.vocatch.main.PersistanceConstants;
import java.nio.file.FileSystems;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.WatchService;
import java.util.logging.Logger;
import static java.nio.file.StandardWatchEventKinds.*;
import java.nio.file.WatchEvent;
import java.nio.file.WatchKey;

/**
 *
 * @author Александр
 */
public final class EnvReloader implements Runnable {

	private final Env env;

	public EnvReloader(Env env) {
		this.env = env;
	}

	@Override
	public void run() {
		try {
			WatchService watcher = FileSystems.getDefault().newWatchService();
			Path envPath = Paths.get(PersistanceConstants.ENV__FOLDER);
			envPath.register(watcher, ENTRY_MODIFY);
			while (true) {
				WatchKey key = watcher.take();
				boolean reloadNeeded = false;
				for (WatchEvent<?> event: key.pollEvents()) {
					WatchEvent.Kind<?> kind = event.kind();
					if (kind == ENTRY_MODIFY) {
						reloadNeeded = true;
						break;
					}
				}
				if (reloadNeeded) {
					env.load();
				}
				key.reset();
			}
		} catch (Exception ex) {
			Logger.getLogger(PersistanceConstants.LOGGER__NAME).severe(ex.getMessage());
		}
	}
}
