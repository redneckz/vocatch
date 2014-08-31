package com.qbit.vocatch.view;

import java.awt.BorderLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.util.logging.Filter;
import java.util.logging.Formatter;
import java.util.logging.Level;
import java.util.logging.LogRecord;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;

import com.qbit.vocatch.main.PersistanceConstants;

public class LogFrame extends JFrame implements Filter {

	private static final long serialVersionUID = 1L;

	private JPanel jContentPane = null;

	private JTextArea logTextArea = null;

	private JScrollPane jScrollPane = null;

	private JButton clearLogWindowButton = null;

	private JPanel bPanel = null;

	private JButton clearLogButton = null;

	public LogFrame() {
		super();
		initialize();
	}

	/**
	 * This method initializes this
	 *
	 * @return void
	 */
	private void initialize() {
		this.setTitle("Лог");
		this.setBounds(new Rectangle(100, 100, 400, 500));
		this.setContentPane(getJContentPane());
		this.setIconImage(Toolkit.getDefaultToolkit().getImage(
				getClass().getResource("/voc.gif")));
	}

	/**
	 * This method initializes jContentPane
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			jContentPane = new JPanel();
			jContentPane.setLayout(new BorderLayout());
			jContentPane.add(getJScrollPane(), BorderLayout.CENTER);
			jContentPane.add(getBPanel(), BorderLayout.SOUTH);
		}
		return jContentPane;
	}

	/**
	 * This method initializes jScrollPane
	 *
	 * @return javax.swing.JScrollPane
	 */
	private JScrollPane getJScrollPane() {
		if (jScrollPane == null) {
			jScrollPane = new JScrollPane();
			jScrollPane.setViewportView(getLogTextArea());
		}
		return jScrollPane;
	}

	/**
	 * This method initializes logTextArea
	 *
	 * @return javax.swing.JTextArea
	 */
	private JTextArea getLogTextArea() {
		if (logTextArea == null) {
			logTextArea = new JTextArea();
			logTextArea.setEditable(false);
		}
		return logTextArea;
	}

	/**
	 * This method initializes bPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getBPanel() {
		if (bPanel == null) {
			GridBagConstraints gridBagConstraints1 = new GridBagConstraints();
			gridBagConstraints1.insets = new Insets(5, 5, 5, 5);
			gridBagConstraints1.fill = GridBagConstraints.NONE;
			gridBagConstraints1.anchor = GridBagConstraints.CENTER;
			GridBagConstraints gridBagConstraints = new GridBagConstraints();
			gridBagConstraints.gridx = 1;
			gridBagConstraints.insets = new Insets(5, 0, 5, 5);
			gridBagConstraints.weighty = 0.0D;
			gridBagConstraints.gridy = 0;
			bPanel = new JPanel();
			bPanel.setLayout(new GridBagLayout());
			bPanel.add(getClearLogWindowButton(), gridBagConstraints1);
			bPanel.add(getClearLogButton(), gridBagConstraints);
		}
		return bPanel;
	}

	/**
	 * This method initializes clearLogWindowButton
	 *
	 * @return javax.swing.JButton
	 */
	private JButton getClearLogWindowButton() {
		if (clearLogWindowButton == null) {
			clearLogWindowButton = new JButton();
			clearLogWindowButton.setText("Очистить окно");
			clearLogWindowButton
					.addActionListener(new java.awt.event.ActionListener() {

						@Override
						public void actionPerformed(java.awt.event.ActionEvent e) {
							Document doc = logTextArea.getDocument();
							try {
								doc.remove(0, doc.getLength());
							} catch (BadLocationException e1) {
							}
						}

					});
		}
		return clearLogWindowButton;
	}

	private boolean en = true;

	/**
	 * This method initializes clearLogButton
	 *
	 * @return javax.swing.JButton
	 */
	private JButton getClearLogButton() {
		if (clearLogButton == null) {
			clearLogButton = new JButton();
			clearLogButton.setText("Откл.");
			clearLogButton
					.addActionListener(new java.awt.event.ActionListener() {
						@Override
						public void actionPerformed(java.awt.event.ActionEvent e) {
							Logger logger = Logger.getLogger(PersistanceConstants.LOGGER__NAME);
							if (en) {
								logger.setLevel(Level.OFF);
								clearLogButton.setText("Вкл.");
							} else {
								logger.setLevel(Level.ALL);
								clearLogButton.setText("Откл.");
							}
							en = !en;
						}
					});
		}
		return clearLogButton;
	}

	private final Formatter formatter = new SimpleFormatter();

	@Override
	public boolean isLoggable(LogRecord record) {
		Document doc = logTextArea.getDocument();
		try {
			if (doc.getLength() > 2047) {
				doc.remove(0, 512);
			}
			doc.insertString(doc.getLength(), formatter.format(record), null);
		} catch (BadLocationException e) {
		}
		return true;
	}

} // @jve:decl-index=0:visual-constraint="9,-4"
