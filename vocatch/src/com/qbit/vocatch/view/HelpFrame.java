package com.qbit.vocatch.view;

import java.awt.BorderLayout;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.io.File;
import java.util.logging.Logger;

import javax.swing.JEditorPane;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;
import javax.swing.text.html.HTMLDocument;
import javax.swing.text.html.HTMLFrameHyperlinkEvent;

import com.qbit.vocatch.main.PersistanceConstants;

public class HelpFrame extends JFrame {

	private static final long serialVersionUID = 1L;

	private JPanel jContentPane = null;

	private JScrollPane helpScrollPane = null;

	private JEditorPane helpEditorPane = null;

	/**
	 * This is the default constructor
	 */
	public HelpFrame() {
		super();
		initialize();
	}

	/**
	 * This method initializes this
	 *
	 * @return void
	 */
	private void initialize() {
		this.setIconImage(Toolkit.getDefaultToolkit().getImage(getClass().getResource("/voc.gif")));
		this.setBounds(new Rectangle(100, 100, 547, 500));
		this.setContentPane(getJContentPane());
		this.setTitle("Справка");
		this.setExtendedState(MAXIMIZED_BOTH);
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
			jContentPane.add(getHelpScrollPane(), BorderLayout.CENTER);
		}
		return jContentPane;
	}

	/**
	 * This method initializes helpScrollPane
	 *
	 * @return javax.swing.JScrollPane
	 */
	private JScrollPane getHelpScrollPane() {
		if (helpScrollPane == null) {
			helpScrollPane = new JScrollPane();
			helpScrollPane.setViewportView(getHelpEditorPane());
		}
		return helpScrollPane;
	}

	/**
	 * This method initializes helpEditorPane
	 *
	 * @return javax.swing.JEditorPane
	 */
	private JEditorPane getHelpEditorPane() {
		if (helpEditorPane == null) {
			helpEditorPane = new JEditorPane();
			helpEditorPane.setContentType("text/html");
			helpEditorPane.setEditable(false);
			try {
				helpEditorPane.setPage(new File("./help/index.html").toURI().toURL());
			} catch (Throwable e) {
				Logger.getLogger(PersistanceConstants.LOGGER__NAME).severe(
						e.getMessage());
			}
			helpEditorPane.addHyperlinkListener(new HyperlinkListener() {

				@Override
				public void hyperlinkUpdate(HyperlinkEvent event) {
					if (event.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
						JEditorPane pane = (JEditorPane) event.getSource();
						if (event instanceof HTMLFrameHyperlinkEvent) {
							HTMLFrameHyperlinkEvent evt = (HTMLFrameHyperlinkEvent) event;
							HTMLDocument doc = (HTMLDocument) pane.getDocument();
							doc.processHTMLFrameHyperlinkEvent(evt);
						} else {
							try {
								pane.setPage(event.getURL());
							} catch (Throwable e) {
								Logger.getLogger(PersistanceConstants.LOGGER__NAME)
										.severe(e.getMessage());
							}
						}
					}
				}
			});
		}
		return helpEditorPane;
	}

} // @jve:decl-index=0:visual-constraint="10,10"
