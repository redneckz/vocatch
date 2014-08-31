package com.qbit.vocatch.view;

import java.awt.Dimension;
import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import com.qbit.vocatch.worker.Worker;
import javax.swing.JCheckBox;

public class SaveSlidesDialog extends JDialog {

	private static final long serialVersionUID = 1L;

	private JPanel jContentPane = null;

	private JLabel slideIndexLabel = null;

	private JSlider slideOffsetSlider = null;

	private JLabel slideCountLabel = null;

	private JSlider slideCountSlider = null;

	private JLabel prefixLabel = null;

	private JTextField prefixTextField = null;

	private JCheckBox withDataCheckBox = null;

	private JPanel jPanel = null;

	private JButton okButton = null;

	private JButton cancelButton = null;

	private boolean ok = false;

	private final int currentSlideIndex;

	private final Worker worker;

	/**
	 * @param owner
	 * @param currentSlideIndex
	 * @param worker
	 */
	public SaveSlidesDialog(Frame owner, int currentSlideIndex, Worker worker) {
		super(owner);
		this.currentSlideIndex = currentSlideIndex;
		this.worker = worker;
		initialize();
	}

	public boolean isOk() {
		return ok;
	}

	public String getFilePrefix() {
		return prefixTextField.getText();
	}

	public int getSlideOffset() {
		return slideOffsetSlider.getValue();
	}

	public int getSlideCount() {
		return slideCountSlider.getValue();
	}

	public boolean isWithData() {
		return withDataCheckBox.isSelected();
	}

	/**
	 * This method initializes this
	 *
	 * @return void
	 */
	private void initialize() {
		this.setTitle("Сохранения кадров");
		this.setModal(true);
		this.setBounds(100, 100, 500, 200);
		this.setContentPane(getJContentPane());
	}

	/**
	 * This method initializes jContentPane
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			GridBagConstraints gridBagConstraints12 = new GridBagConstraints();
			gridBagConstraints12.gridx = 0;
			gridBagConstraints12.anchor = GridBagConstraints.WEST;
			gridBagConstraints12.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints12.gridwidth = 2;
			gridBagConstraints12.gridy = 3;
			GridBagConstraints gridBagConstraints11 = new GridBagConstraints();
			gridBagConstraints11.gridx = 0;
			gridBagConstraints11.fill = GridBagConstraints.BOTH;
			gridBagConstraints11.weightx = 1.0D;
			gridBagConstraints11.weighty = 1.0D;
			gridBagConstraints11.gridwidth = 2;
			gridBagConstraints11.insets = new Insets(0, 5, 5, 5);
			gridBagConstraints11.gridy = 4;
			GridBagConstraints gridBagConstraints6 = new GridBagConstraints();
			gridBagConstraints6.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints6.gridy = 2;
			gridBagConstraints6.weightx = 1.0;
			gridBagConstraints6.insets = new Insets(0, 0, 5, 5);
			gridBagConstraints6.gridx = 1;
			GridBagConstraints gridBagConstraints5 = new GridBagConstraints();
			gridBagConstraints5.gridx = 0;
			gridBagConstraints5.fill = GridBagConstraints.BOTH;
			gridBagConstraints5.insets = new Insets(0, 5, 5, 5);
			gridBagConstraints5.gridy = 2;
			prefixLabel = new JLabel();
			prefixLabel.setText("Префикс:");
			GridBagConstraints gridBagConstraints3 = new GridBagConstraints();
			gridBagConstraints3.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints3.gridy = 1;
			gridBagConstraints3.weightx = 1.0;
			gridBagConstraints3.insets = new Insets(0, 0, 5, 5);
			gridBagConstraints3.gridx = 1;
			GridBagConstraints gridBagConstraints2 = new GridBagConstraints();
			gridBagConstraints2.gridx = 0;
			gridBagConstraints2.insets = new Insets(0, 5, 5, 5);
			gridBagConstraints2.anchor = GridBagConstraints.CENTER;
			gridBagConstraints2.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints2.gridy = 1;
			slideCountLabel = new JLabel();
			slideCountLabel.setText("Кол-во кадров:");
			GridBagConstraints gridBagConstraints1 = new GridBagConstraints();
			gridBagConstraints1.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints1.gridy = 0;
			gridBagConstraints1.weightx = 1.0;
			gridBagConstraints1.anchor = GridBagConstraints.CENTER;
			gridBagConstraints1.insets = new Insets(5, 0, 5, 5);
			gridBagConstraints1.gridx = 1;
			GridBagConstraints gridBagConstraints = new GridBagConstraints();
			gridBagConstraints.gridx = 0;
			gridBagConstraints.insets = new Insets(5, 5, 5, 5);
			gridBagConstraints.anchor = GridBagConstraints.CENTER;
			gridBagConstraints.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints.gridy = 0;
			slideIndexLabel = new JLabel();
			slideIndexLabel.setText("Номер кадра:");
			jContentPane = new JPanel();
			jContentPane.setLayout(new GridBagLayout());
			jContentPane.add(slideCountLabel, gridBagConstraints2);
			jContentPane.add(getSlideCountSlider(), gridBagConstraints3);
			jContentPane.add(slideIndexLabel, gridBagConstraints);
			jContentPane.add(getSlideOffsetSlider(), gridBagConstraints1);
			jContentPane.add(prefixLabel, gridBagConstraints5);
			jContentPane.add(getPrefixTextField(), gridBagConstraints6);
			jContentPane.add(getJPanel(), gridBagConstraints11);
			jContentPane.add(getWithDataCheckBox(), gridBagConstraints12);
		}
		return jContentPane;
	}

	/**
	 * This method initializes slideIndexSlider
	 *
	 * @return javax.swing.JSlider
	 */
	private JSlider getSlideOffsetSlider() {
		if (slideOffsetSlider == null) {
			slideOffsetSlider = new JSlider();
			slideOffsetSlider.setMinimum(0);
			slideOffsetSlider.setMaximum(worker.getSlideCount() - 1);
			slideOffsetSlider.setMinorTickSpacing(10);
			slideOffsetSlider.setPaintTicks(true);
			slideOffsetSlider.setPaintLabels(false);
			slideOffsetSlider.setSnapToTicks(false);
			slideOffsetSlider.setMajorTickSpacing(100);
			slideOffsetSlider.addChangeListener(new ChangeListener() {

				@Override
				public void stateChanged(ChangeEvent e) {
					slideIndexLabel.setText("Номер кадра [" + slideOffsetSlider.getValue() + "]:");
					slideCountSlider.setValue(1);
					slideCountSlider.setMaximum(worker.getSlideCount() - slideOffsetSlider.getValue());
				}

			});
			slideOffsetSlider.setValue(currentSlideIndex);
		}
		return slideOffsetSlider;
	}

	/**
	 * This method initializes slideCountSlider
	 *
	 * @return javax.swing.JSlider
	 */
	private JSlider getSlideCountSlider() {
		if (slideCountSlider == null) {
			slideCountSlider = new JSlider();
			slideCountSlider.setMinimum(1);
			slideCountSlider.setMaximum(worker.getSlideCount() - currentSlideIndex);
			slideCountSlider.setPaintLabels(false);
			slideCountSlider.setPaintTicks(true);
			slideCountSlider.setSnapToTicks(false);
			slideCountSlider.setMinorTickSpacing(10);
			slideCountSlider.setMajorTickSpacing(100);
			slideCountSlider.setValue(1);
			slideCountSlider.setPaintLabels(true);
			slideCountSlider.addChangeListener(new ChangeListener() {

				@Override
				public void stateChanged(ChangeEvent e) {
					slideCountLabel.setText("Кол-во кадров [" + slideCountSlider.getValue() + "]:");
				}

			});
		}
		return slideCountSlider;
	}

	/**
	 * This method initializes prefixTextField
	 *
	 * @return javax.swing.JTextField
	 */
	private JTextField getPrefixTextField() {
		if (prefixTextField == null) {
			prefixTextField = new JTextField();
		}
		return prefixTextField;
	}

	/**
	 * This method initializes jPanel
	 *
	 * @return javax.swing.JPanel
	 */
	private JPanel getJPanel() {
		if (jPanel == null) {
			jPanel = new JPanel();
			jPanel.setLayout(new BoxLayout(getJPanel(), BoxLayout.X_AXIS));
			jPanel.add(Box.createHorizontalGlue());
			jPanel.add(getOkButton(), null);
			jPanel.add(Box.createRigidArea(new Dimension(5, 5)));
			jPanel.add(getCancelButton(), null);
		}
		return jPanel;
	}

	/**
	 * This method initializes okButton
	 *
	 * @return javax.swing.JButton
	 */
	private JButton getOkButton() {
		if (okButton == null) {
			okButton = new JButton();
			okButton.setText("OK");
			okButton.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent e) {
					ok = true;
					SaveSlidesDialog.this.dispose();
				}

			});
		}
		return okButton;
	}

	/**
	 * This method initializes cancelButton
	 *
	 * @return javax.swing.JButton
	 */
	private JButton getCancelButton() {
		if (cancelButton == null) {
			cancelButton = new JButton();
			cancelButton.setText("Отмена");
			cancelButton.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent e) {
					ok = false;
					SaveSlidesDialog.this.dispose();
				}

			});
		}
		return cancelButton;
	}

	/**
	 * This method initializes withDataCheckBox
	 *
	 * @return javax.swing.JCheckBox
	 */
	private JCheckBox getWithDataCheckBox() {
		if (withDataCheckBox == null) {
			withDataCheckBox = new JCheckBox();
			withDataCheckBox.setText("Вместе с поправками");
			withDataCheckBox.setSelected(true);
		}
		return withDataCheckBox;
	}

} // @jve:decl-index=0:visual-constraint="10,10"
