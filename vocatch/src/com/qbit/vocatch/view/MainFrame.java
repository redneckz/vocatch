package com.qbit.vocatch.view;

import com.qbit.vocatch.env.Env;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.logging.Logger;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JToggleButton;
import javax.swing.SwingUtilities;
import javax.swing.Timer;

import com.qbit.vocatch.main.PersistanceConstants;
import com.qbit.vocatch.modules.SlideIO;
import com.qbit.vocatch.worker.Worker;
import com.qbit.vocatch.worker.WorkerException;

public class MainFrame extends JFrame {

	private static final long serialVersionUID = 1L;
	
	private final Env env;

	private final Worker worker;

	private JPanel jContentPane = null;

	private JPanel slidePanel = null;

	private JToggleButton runToggleButton = null;

	private JButton prevSlideButton = null;

	private JToggleButton playBButton = null;

	private JToggleButton playFButton = null;

	private JButton nextSlideButton = null;

	private JButton saveSlideButton = null;

	private JProgressBar waitingSCProgressBar = null;

	private int currentSlideIndex = 0;

	/**
	 * This is the default constructor
	 * @param env
	 * @param worker
	 */
	public MainFrame(Env env, Worker worker) {
		super();
		this.env = env;
		this.worker = worker;
		initialize();
		worker.addPropertyChangeListener(Worker.PNAME__WORK_STARTED,
				new PropertyChangeListener() {

					@Override
					public void propertyChange(final PropertyChangeEvent evt) {
						Logger.getLogger(PersistanceConstants.LOGGER__NAME).info("Цикл запущен.");
						currentSlideIndex = 0;
						SwingUtilities.invokeLater(new Runnable() {

							@Override
							public void run() {
								prevSlideButton.setEnabled(false);
								playBButton.setEnabled(false);
								playFButton.setEnabled(false);
								nextSlideButton.setEnabled(false);
								saveSlideButton.setEnabled(false);
							}
						});
					}

				});
		worker.addPropertyChangeListener(Worker.PNAME__NEXT_SLIDE_READY,
				new PropertyChangeListener() {

					@Override
					public void propertyChange(final PropertyChangeEvent evt) {
						currentSlideIndex = (Integer) evt.getNewValue();
						SwingUtilities.invokeLater(new Runnable() {

							@Override
							public void run() {
								slidePanel.repaint();
							}
						});
					}

				});
		worker.addPropertyChangeListener(Worker.PNAME__WAITING_SC_CHANGED,
				new PropertyChangeListener() {

					@Override
					public void propertyChange(final PropertyChangeEvent evt) {
						final int a = (Integer) evt.getNewValue();
						final int b = (Integer) evt.getOldValue();
						SwingUtilities.invokeLater(new Runnable() {

							@Override
							public void run() {
								waitingSCProgressBar.setValue(a * 100 / b);
							}
						});
					}

				});
		worker.addPropertyChangeListener(Worker.PNAME__WORK_ENDED,
				new PropertyChangeListener() {

					@Override
					public void propertyChange(final PropertyChangeEvent evt) {
						Logger.getLogger(PersistanceConstants.LOGGER__NAME).info("Цикл завершен.");
						currentSlideIndex = 0;
						SwingUtilities.invokeLater(new Runnable() {
							@Override
							public void run() {
								prevSlideButton.setEnabled(true);
								playBButton.setEnabled(true);
								playFButton.setEnabled(true);
								nextSlideButton.setEnabled(true);
								saveSlideButton.setEnabled(true);
								slidePanel.repaint();
							}
						});
					}

				});
	}

	/**
	 * This method initializes this
	 *
	 * @return void
	 */
	private void initialize() {
		this.setIconImage(Toolkit.getDefaultToolkit().getImage(getClass().getResource("/voc.gif")));
		this.setBounds(new Rectangle(100, 100, 480, 500));
		this.setContentPane(getJContentPane());
		this.setTitle("[vocatch]");
	}

	/**
	 * This method initializes jContentPane
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			GridBagConstraints gridBagConstraints14 = new GridBagConstraints();
			gridBagConstraints14.gridx = 0;
			gridBagConstraints14.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints14.gridwidth = 6;
			gridBagConstraints14.gridy = 2;
			GridBagConstraints gridBagConstraints12 = new GridBagConstraints();
			gridBagConstraints12.gridx = 5;
			gridBagConstraints12.insets = new Insets(5, 0, 5, 5);
			gridBagConstraints12.gridy = 1;
			GridBagConstraints gridBagConstraints7 = new GridBagConstraints();
			gridBagConstraints7.gridx = 4;
			gridBagConstraints7.insets = new Insets(5, 0, 5, 5);
			gridBagConstraints7.gridy = 1;
			GridBagConstraints gridBagConstraints6 = new GridBagConstraints();
			gridBagConstraints6.gridx = 3;
			gridBagConstraints6.insets = new Insets(5, 0, 5, 5);
			gridBagConstraints6.gridy = 1;
			GridBagConstraints gridBagConstraints5 = new GridBagConstraints();
			gridBagConstraints5.gridx = 2;
			gridBagConstraints5.insets = new Insets(5, 0, 5, 5);
			gridBagConstraints5.gridy = 1;
			GridBagConstraints gridBagConstraints11 = new GridBagConstraints();
			gridBagConstraints11.gridx = 1;
			gridBagConstraints11.insets = new Insets(5, 5, 5, 5);
			gridBagConstraints11.gridy = 1;
			GridBagConstraints gridBagConstraints1 = new GridBagConstraints();
			gridBagConstraints1.insets = new Insets(5, 5, 5, 5);
			gridBagConstraints1.gridy = 1;
			gridBagConstraints1.fill = GridBagConstraints.NONE;
			gridBagConstraints1.weighty = 0.0D;
			gridBagConstraints1.anchor = GridBagConstraints.WEST;
			gridBagConstraints1.weightx = 1.0D;
			gridBagConstraints1.gridx = 0;
			GridBagConstraints gridBagConstraints = new GridBagConstraints();
			gridBagConstraints.gridheight = 1;
			gridBagConstraints.gridx = 0;
			gridBagConstraints.gridy = 0;
			gridBagConstraints.fill = GridBagConstraints.BOTH;
			gridBagConstraints.weightx = 1.0D;
			gridBagConstraints.weighty = 1.0D;
			gridBagConstraints.gridwidth = 6;
			jContentPane = new JPanel();
			jContentPane.setLayout(new GridBagLayout());
			jContentPane.add(getSlidePanel(), gridBagConstraints);
			jContentPane.add(getRunToggleButton(), gridBagConstraints1);
			jContentPane.add(getPrevSlideButton(), gridBagConstraints11);
			jContentPane.add(getPlayBButton(), gridBagConstraints5);
			jContentPane.add(getPlayFButton(), gridBagConstraints6);
			jContentPane.add(getNextSlideButton(), gridBagConstraints7);
			jContentPane.add(getSaveSlideButton(), gridBagConstraints12);
			jContentPane.add(getWaitingSCProgressBar(), gridBagConstraints14);
		}
		return jContentPane;
	}

	@SuppressWarnings("serial")
	private class SlidePanel extends JPanel {

		@Override
		public void paint(Graphics g) {
			try {
				SlideIO.drawSlide(worker.getSlideByIndex(currentSlideIndex), true, (Graphics2D) g, this);
			} catch (Exception e) {
				Logger.getLogger(PersistanceConstants.LOGGER__NAME).severe(e.getMessage());
			}
		}

	}

	/**
	 * This method initializes slidePanel
	 *
	 * @return javax.swing.JPanel
	 */
	@SuppressWarnings("serial")
	private JPanel getSlidePanel() {
		if (slidePanel == null) {
			slidePanel = new SlidePanel();
			slidePanel.setLayout(new GridBagLayout());
			slidePanel.setBackground(Color.white);
		}
		return slidePanel;
	}

	/**
	 * This method initializes runToggleButton
	 *
	 * @return javax.swing.JToggleButton
	 */
	private JToggleButton getRunToggleButton() {
		if (runToggleButton == null) {
			runToggleButton = new JToggleButton();
			runToggleButton.setIcon(new ImageIcon(getClass().getResource("/rb.gif")));
			runToggleButton.setRolloverEnabled(true);
			runToggleButton.setRolloverIcon(new ImageIcon(getClass().getResource("/rbp.gif")));
			runToggleButton.setSelectedIcon(new ImageIcon(getClass().getResource("/rbs.gif")));
			runToggleButton.addItemListener(new java.awt.event.ItemListener() {
				
				@Override
				public void itemStateChanged(java.awt.event.ItemEvent e) {
					if (runToggleButton.isSelected()) {
						try {
							if (!worker.work()) {
								runToggleButton.setSelected(false);
							}
						} catch (WorkerException ex) {
							Logger.getLogger(PersistanceConstants.LOGGER__NAME)
									.severe(ex.getMessage());
						}
					} else {
						worker.stop();
					}
				}
			});
		}
		return runToggleButton;
	}

	/**
	 * This method initializes prevSlideButton
	 *
	 * @return javax.swing.JButton
	 */
	private JButton getPrevSlideButton() {
		if (prevSlideButton == null) {
			prevSlideButton = new JButton();
			prevSlideButton.setEnabled(false);
			prevSlideButton.setText("<");
			prevSlideButton.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent e) {
					currentSlideIndex--;
					if (currentSlideIndex < 0) {
						currentSlideIndex = worker.getSlideCount() - 1;
					}
					slidePanel.repaint();
				}

			});
		}
		return prevSlideButton;
	}

	private final Timer rTimer = new Timer(40, new ActionListener() {

		@Override
		public void actionPerformed(ActionEvent e) {
			currentSlideIndex--;
			if (currentSlideIndex < 0) {
				currentSlideIndex = worker.getSlideCount() - 1;
			}
			slidePanel.repaint();
		}

	}); // @jve:decl-index=0:

	/**
	 * This method initializes playBButton
	 *
	 * @return javax.swing.JToggleButton
	 */
	private JToggleButton getPlayBButton() {
		if (playBButton == null) {
			playBButton = new JToggleButton();
			playBButton.setEnabled(false);
			playBButton.setText("<<");
			playBButton.addItemListener(new java.awt.event.ItemListener() {
				
				@Override
				public void itemStateChanged(java.awt.event.ItemEvent e) {
					boolean isSelected = playBButton.isSelected();
					runToggleButton.setEnabled(!isSelected);
					prevSlideButton.setEnabled(!isSelected);
					playFButton.setEnabled(!isSelected);
					nextSlideButton.setEnabled(!isSelected);
					saveSlideButton.setEnabled(!isSelected);
					if (isSelected) {
						rTimer.start();
					} else {
						rTimer.stop();
					}
				}
			});
		}
		return playBButton;
	}

	private final Timer fTimer = new Timer(40, new ActionListener() {

		@Override
		public void actionPerformed(ActionEvent e) {
			currentSlideIndex = (currentSlideIndex + 1) % worker.getSlideCount();
			slidePanel.repaint();
		}

	}); // @jve:decl-index=0:

	/**
	 * This method initializes playFButton
	 *
	 * @return javax.swing.JToggleButton
	 */
	private JToggleButton getPlayFButton() {
		if (playFButton == null) {
			playFButton = new JToggleButton();
			playFButton.setEnabled(false);
			playFButton.setText(">>");
			playFButton.addItemListener(new java.awt.event.ItemListener() {
				
				@Override
				public void itemStateChanged(java.awt.event.ItemEvent e) {
					boolean isSelected = playFButton.isSelected();
					runToggleButton.setEnabled(!isSelected);
					prevSlideButton.setEnabled(!isSelected);
					playBButton.setEnabled(!isSelected);
					nextSlideButton.setEnabled(!isSelected);
					saveSlideButton.setEnabled(!isSelected);
					if (isSelected) {
						fTimer.start();
					} else {
						fTimer.stop();
					}
				}
			});
		}
		return playFButton;
	}

	/**
	 * This method initializes nextSlideButton
	 *
	 * @return javax.swing.JButton
	 */
	private JButton getNextSlideButton() {
		if (nextSlideButton == null) {
			nextSlideButton = new JButton();
			nextSlideButton.setEnabled(false);
			nextSlideButton.setText(">");
			nextSlideButton.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent e) {
					currentSlideIndex = (currentSlideIndex + 1) % worker.getSlideCount();
					slidePanel.repaint();
				}

			});
		}
		return nextSlideButton;
	}

	/**
	 * This method initializes saveSlideButton
	 *
	 * @return javax.swing.JButton
	 */
	private JButton getSaveSlideButton() {
		if (saveSlideButton == null) {
			saveSlideButton = new JButton();
			saveSlideButton.setEnabled(false);
			saveSlideButton.setToolTipText("Сохранить слайд");
			saveSlideButton.setIcon(new ImageIcon(getClass().getResource("/ed_save.gif")));
			saveSlideButton.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent e) {
					if (!worker.isWorking() && worker.getSlideCount() > 0) {
						SaveSlidesDialog saveSlidesDialog = new SaveSlidesDialog(
								MainFrame.this, currentSlideIndex, worker);
						saveSlidesDialog.setVisible(true);
						if (saveSlidesDialog.isOk()) {
							try {
								SlideIO.saveSlides(worker,
										saveSlidesDialog.getSlideOffset(),
										saveSlidesDialog.getSlideCount(),
										saveSlidesDialog.isWithData(),
										saveSlidesDialog.getFilePrefix(),
										env.getWorkingDirectory());
								Logger.getLogger(PersistanceConstants.LOGGER__NAME).info("Сохранение кадров успепшно!");
							} catch (Exception ex) {
								Logger.getLogger(PersistanceConstants.LOGGER__NAME).severe(ex.getMessage());
							}
						}
					}
				}

			});
		}
		return saveSlideButton;
	}

	/**
	 * This method initializes waitingSCProgressBar
	 *
	 * @return javax.swing.JProgressBar
	 */
	private JProgressBar getWaitingSCProgressBar() {
		if (waitingSCProgressBar == null) {
			waitingSCProgressBar = new JProgressBar();
			waitingSCProgressBar.setStringPainted(true);
		}
		return waitingSCProgressBar;
	}

} // @jve:decl-index=0:visual-constraint="10,10"
