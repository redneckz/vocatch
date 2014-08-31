package com.qbit.vocatch.main;


import com.qbit.vocatch.env.Env;
import java.awt.AWTException;
import java.awt.Image;
import java.awt.MenuItem;
import java.awt.PopupMenu;
import java.awt.SystemTray;
import java.awt.Toolkit;
import java.awt.TrayIcon;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.logging.Logger;

import javax.swing.JOptionPane;

import com.qbit.vocatch.factory.SystemFactory;
import com.qbit.vocatch.factory.impl.DefaultSystemFactory;
import com.qbit.vocatch.proxy.Command;
import com.qbit.vocatch.proxy.CommandListener;
import com.qbit.vocatch.view.HelpFrame;
import com.qbit.vocatch.view.LogFrame;
import com.qbit.vocatch.view.MainFrame;
import com.qbit.vocatch.worker.Worker;

public final class VOCMain {

	public static void main(String[] args) throws Exception {
		if (SystemTray.isSupported()) {
			init(DefaultSystemFactory.inst());
		} else {
			showErrorMessage("Поддержка java.awt.SystemTray отсутствует", false);
		}
	}

	private static void showErrorMessage(String message, boolean exit) {
		JOptionPane.showMessageDialog(null, message, "Ошибка", JOptionPane.ERROR_MESSAGE);
		if (exit) {
			System.exit(1);
		}
	}

	private static void init(final SystemFactory factory) throws Exception {
		final Env env = factory.createEnv();
		final Logger logger = factory.configureLogger();
		final Worker worker = factory.createWorker();
		
		final MainFrame mainFrame = new MainFrame(env, worker);
		final HelpFrame helpFrame = new HelpFrame();
		final LogFrame logFrame = new LogFrame();
		logger.setFilter(logFrame);
		PopupMenu menu = new PopupMenu();
		final MenuItem menuItemForControl = new MenuItem("Управление");
		menuItemForControl.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				mainFrame.setVisible(true);
			}

		});
		menu.add(menuItemForControl);
		menu.addSeparator();
		final MenuItem menuItemForLog = new MenuItem("Лог");
		menuItemForLog.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				logFrame.setVisible(true);
			}

		});
		menu.add(menuItemForLog);
		final MenuItem menuItemForHelp = new MenuItem("Справка");
		menuItemForHelp.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				helpFrame.setVisible(true);
			}

		});
		menu.add(menuItemForHelp);
		menu.addSeparator();
		MenuItem menuItemForExit = new MenuItem("Выход");
		menuItemForExit.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				System.exit(0);
			}

		});
		menu.add(menuItemForExit);
		Image trayImage = Toolkit.getDefaultToolkit().getImage(menu.getClass().getResource("/voc.gif"));
		TrayIcon trayIcon = new TrayIcon(trayImage, "VOC", menu);
		trayIcon.setImageAutoSize(true);
		
		try {
			final SystemTray tray = SystemTray.getSystemTray();
			tray.add(trayIcon);
		} catch (AWTException e) {
			showErrorMessage("java.awt.SystemTray работает некорректно", true);
		}
		
		factory.createComCenterProxy(new CommandListener() {

			@Override
			public void onCommand(Command cmd) {
				logger.info(cmd.name());
			}

		});
		
		if (Constants.DEBUG) {
			mainFrame.setVisible(true);
			logFrame.setVisible(true);
		}
	}

}
