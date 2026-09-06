import org.w3c.dom.*;
import javax.xml.parsers.*;
import java.io.*;

/**
 * Parses invoice XML files uploaded by third-party vendors through the
 * partner portal, and extracts line-item totals for reconciliation.
 */
public class InvoiceXmlParser {

    public double parseInvoiceTotal(InputStream vendorUploadedXml) throws Exception {
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        DocumentBuilder builder = dbf.newDocumentBuilder();
        Document doc = builder.parse(vendorUploadedXml);

        NodeList lineItems = doc.getElementsByTagName("lineItem");
        double total = 0.0;

        for (int i = 0; i < lineItems.getLength(); i++) {
            Element item = (Element) lineItems.item(i);
            String amountStr = item.getElementsByTagName("amount")
                    .item(0).getTextContent();
            total += Double.parseDouble(amountStr);
        }

        return total;
    }

    public static void main(String[] args) throws Exception {
        InvoiceXmlParser parser = new InvoiceXmlParser();
        try (FileInputStream fis = new FileInputStream(args[0])) {
            double total = parser.parseInvoiceTotal(fis);
            System.out.println("Invoice total: $" + total);
        }
    }
}
