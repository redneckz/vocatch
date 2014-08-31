package com.qbit.vocatch.modules;

import com.qbit.vocatch.main.PersistanceConstants;
import com.qbit.vocatch.worker.WorkerException;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Component;
import java.awt.Font;
import java.awt.Graphics2D;
import java.awt.Paint;
import java.awt.Shape;
import java.awt.Stroke;
import java.awt.font.FontRenderContext;
import java.awt.geom.AffineTransform;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.File;
import static java.lang.Math.PI;
import static java.lang.Math.abs;
import static java.lang.Math.cos;
import static java.lang.Math.sin;
import static java.lang.Math.tan;
import java.text.DateFormat;
import java.util.Date;
import java.util.logging.Logger;
import javax.imageio.ImageIO;

public final class SlideIO {
	
	public static final String SLIDE_IMAGE__FORMAT_NAME = "png";
	
	private static final Paint PAINT_FOR_COFM = Color.RED;

	private static final Paint PAINT_FOR_LINES0 = Color.RED;

	private static final Paint PAINT_FOR_LINES1 = Color.RED;

	private static final Paint PAINT_FOR_TEXT = Color.RED;

	private static final Stroke STROKE_FOR_LINES0 = new BasicStroke(2.0f,
			BasicStroke.CAP_BUTT, BasicStroke.JOIN_MITER, 10.0f, null, 0.0f);

	private static final Stroke STROKE_FOR_LINES1 = new BasicStroke(2f,
			BasicStroke.CAP_BUTT, BasicStroke.JOIN_MITER, 10.0f, new float[]{10.0f, 4.0f}, 0.0f);

	private SlideIO() {
	}
	
	public static void drawSlide(Slide slide, boolean withData, Graphics2D g2, Object target) {
		if (slide == null) {
			return;
		}
		final int width = slide.getSlideWidth();
		BufferedImage slideImage = new BufferedImage(width, width, BufferedImage.TYPE_BYTE_GRAY);
		slideImage.getRaster().setDataElements(0, 0, width, width, slide.getRatser());
		Paint oldPaint = g2.getPaint();
		Stroke oldStroke = g2.getStroke();
		AffineTransform oldTransform = g2.getTransform();
		if (target != null && target instanceof Component) {
			double targetWidth = ((Component) target).getWidth();
			double targetHeight = ((Component) target).getHeight();
			double scale = Math.min(targetWidth, targetHeight) / (double) width;
			AffineTransform newTransform = (AffineTransform) oldTransform.clone();
			newTransform.concatenate(AffineTransform.getScaleInstance(scale, scale));
			g2.setTransform(newTransform);
		}
		g2.drawImage(slideImage, null, 0, 0);
		if (withData) {
			ObjectParams objectParams = slide.getObjectParams();
			double angle = objectParams.getAngle();
			int angleGrads = (int) (angle * 180.0 / PI);
			int angleMinutes = (int) abs((angle * 180 / PI - angleGrads) * 60.0);
			int objectX = objectParams.getX();
			int objectY = objectParams.getY();
			Shape slideCenterShape = new Ellipse2D.Double(width / 2 - 2, width / 2 - 2, 4, 4);
			Shape cofmShape = new Ellipse2D.Double(objectX - 5, objectY - 5, 10, 10);
			Shape verticalLineShape = new Line2D.Double(objectX, 0, objectX, width);
			double x = objectX + tan(angle) * objectY;
			double y = 0.0;
			if (x < 0.0) {
				x = 0.0;
				y = objectY - tan(PI / 2.0 + angle) * objectX;
			} else if (x > width) {
				x = width;
				y = objectY - tan(PI / 2.0 - angle) * (width - objectX);
			}
			Point2D firstControlLinePoint = new Point2D.Double(x, y);
			x = objectX - tan(angle) * (width - objectY);
			y = width;
			if (x < 0.0) {
				x = 0.0;
				y = objectY + tan(PI / 2.0 - angle) * objectX;
			} else if (x > width) {
				x = width;
				y = objectY + tan(PI / 2.0 + angle) * (width - objectX);
			}
			Point2D secondControlLinePoint = new Point2D.Double(x, y);
			Shape controlLineShape = new Line2D.Double(firstControlLinePoint, secondControlLinePoint);
			g2.setPaint(PAINT_FOR_COFM);
			g2.fill(cofmShape);
			g2.fill(slideCenterShape);
			g2.setPaint(PAINT_FOR_LINES0);
			g2.setStroke(STROKE_FOR_LINES0);
			g2.draw(verticalLineShape);
			g2.setPaint(PAINT_FOR_LINES1);
			g2.setStroke(STROKE_FOR_LINES1);
			g2.draw(controlLineShape);
			g2.setPaint(PAINT_FOR_TEXT);
			Font font = g2.getFont();
			String stringForDraw = "Поправка: " + angleGrads + '\u00B0' + angleMinutes + '\'';
			FontRenderContext fontRenderContext = g2.getFontRenderContext();
			Rectangle2D stringBounds = font.getStringBounds(stringForDraw, fontRenderContext);
			float textY = (float) (stringBounds.getHeight());
			g2.drawGlyphVector(font.createGlyphVector(fontRenderContext, stringForDraw), 2f, textY);
			stringForDraw = "Смещение: " + SlideIO.getObjectOffset(slide);
			stringBounds = font.getStringBounds(stringForDraw, fontRenderContext);
			textY += (float) (stringBounds.getHeight());
			g2.drawGlyphVector(font.createGlyphVector(fontRenderContext, stringForDraw), 2f, textY);
		}
		g2.setTransform(oldTransform);
		g2.setStroke(oldStroke);
		g2.setPaint(oldPaint);
	}

	public static int getObjectOffset(Slide slide) {
		ObjectParams objectParams = slide.getObjectParams();
		double angle = objectParams.getAngle();
		int slideWidth = slide.getParent().getSlideWidth();
		return (int) abs((slideWidth / 2 - objectParams.getX()) * cos(angle)
				+ (slideWidth / 2 - objectParams.getY()) * sin(angle));
	}
	
	public static void saveSlides(SlideBuffer slideBuffer, int slideOffset, int slideCount,
			boolean withData, String filePrefix, File workingDirectory) throws WorkerException {
		try {
			String timestamp = DateFormat.getDateTimeInstance(DateFormat.SHORT, DateFormat.SHORT).format(new Date());
			for (int relSlideIndex = 0; relSlideIndex < slideCount; relSlideIndex++) {
				int slideIndex = slideOffset + relSlideIndex;
				Slide currentSlide = slideBuffer.getSlideByIndex(slideIndex);
				BufferedImage imageForDraw = new BufferedImage(currentSlide.getSlideWidth(),
						currentSlide.getSlideWidth(), BufferedImage.TYPE_INT_ARGB);
				Graphics2D g2 = imageForDraw.createGraphics();
				drawSlide(currentSlide, withData, g2, null);
				g2.dispose();
				File slideFile = new File(workingDirectory, filePrefix + '-' + timestamp
						+ '-' + slideIndex + '.' + SLIDE_IMAGE__FORMAT_NAME);
				ImageIO.write(imageForDraw, SLIDE_IMAGE__FORMAT_NAME, slideFile);
			}
		} catch (Exception ex) {
			Logger.getLogger(PersistanceConstants.LOGGER__NAME).severe(ex.getMessage());
		}
	}
}
