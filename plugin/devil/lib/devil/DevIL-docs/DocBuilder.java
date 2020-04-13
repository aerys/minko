import java.io.*;
import javax.xml.parsers.*;
import java.util.Vector;
import java.util.Stack;
import java.util.Enumeration;
import org.xml.sax.*;
import org.xml.sax.helpers.*;

public class DocBuilder extends DefaultHandler {
	private String  content = null;
	public int      flag = 0;
	
	private PrintStream out = null;
	private Stack   xml_trace = new Stack();
	private int     entity_type;
	private int     content_type = 0;
	private Entity  container;
	private Vector  sets = new Vector();
	private Set     current_set;
	
	private Links   current_link;
	
	class           NamedObject {
		String          name = null;
	}
	
	class Entity extends NamedObject {
		String          description = null;
	}
	
	class LinkedEntity extends Entity {
		Vector          links = new Vector();
		//links
	};
	
	class Value extends LinkedEntity {
	};
	
	class Function extends LinkedEntity {
		String          ret = null;
		Vector          param = new Vector();
	};
	
	class Param extends Entity {
		String          type = null;
	}
	
	class Link extends NamedObject {
		String          from;
		String          description;
	};
	
	class Links extends NamedObject {
		Vector          links = new Vector();
		String          verbose;
	};
	
	class Set extends NamedObject {
		Vector          entity = new Vector();
	};
	
	public void     startElement(String uri, String localName, String rawName, Attributes attributes) {
		xml_trace.push(new String(rawName));
		switch (flag) {
			case 0://DOC
				if (rawName.compareToIgnoreCase("doc") == 0)
					flag = 1;
				else {
					System.out.println("[Parser] Parse Error\nf:" + flag);
					printXMLTrace();
					System.exit(0);
				}
					break;
			case            1://SET
				if (rawName.compareToIgnoreCase("set") == 0) {
					flag = 2;
				} else {
					System.out.println("[Parser] Parse Error\nf:" + flag);
					printXMLTrace();
					System.exit(0);
				}
				
				String set_name = getNamedAttribute("name", attributes);
				//System.out.println("Parsing set:"+set_name);
        current_set = new Set();
				current_set.name = set_name;
				sets.add(current_set);
				break;
			case 2:
				if (rawName.compareToIgnoreCase("entity") == 0) {
					flag = 3;
				} else {
					System.out.println("[Parser] Parse Error\nf:" + flag);
					printXMLTrace();
					System.exit(0);
				}
				
				String type = getNamedAttribute("type", attributes);
				
				if( type.compareToIgnoreCase("value") == 0) {
					entity_type = 0;
					container = new Value();
          //System.out.println("Parsing value");
				} else if (type.compareToIgnoreCase("function") == 0) {
					entity_type = 1;
					container = new Function();
          //System.out.println("Parsing func");
				} else {
					System.out.println("[Parser] Parse Error\nExpected a valid type, found:" + type);
					printXMLTrace();
					System.exit(0);
				}
					break;
			case 3:
        flag=10;
				if (entity_type == 0) {
					if (rawName.compareToIgnoreCase("name") == 0) {
						content_type = 1;
						break;
					} else if (rawName.compareToIgnoreCase("description") == 0) {
						content_type = 2;
						break;
					} else if (rawName.compareToIgnoreCase("link") == 0) {
						current_link = new Links();
						current_link.name = getNamedAttribute("name", attributes);
						current_link.verbose = getNamedAttribute("verbose", attributes);
						
						((Value) container).links.add(current_link);
						content_type = 4;
						flag = 4;
					} else {
						flag = 3;
						System.out.println("[Parser] Parse Error\nf:" + flag + "\nUnexpected Tag " + rawName);
						printXMLTrace();
						System.exit(0);
					}
				} else {
					if (rawName.compareToIgnoreCase("name") == 0) {
						content_type = 1;
					} else if (rawName.compareToIgnoreCase("return") == 0) {
						content_type = 2;
					} else if (rawName.compareToIgnoreCase("description") == 0) {
						content_type = 3;
					} else if (rawName.compareToIgnoreCase("link") == 0) {
						current_link = new Links();
						current_link.name = getNamedAttribute("name", attributes);
						current_link.verbose = getNamedAttribute("verbose", attributes);
						((Function) container).links.add(current_link);
						content_type = 4;
						flag = 4;
					} else if (rawName.compareToIgnoreCase("param") == 0) {
						Param           p = new Param();
						p.name = getNamedAttribute("name", attributes);
						p.description = getNamedAttribute("description", attributes);
						p.type = getNamedAttribute("type", attributes);
						((Function) container).param.add(p);
					} else {
						System.out.println("[Parser] Parse Error\nflag:" + flag + "\nUnexpected Tag " + rawName);
						printXMLTrace();
						System.exit(0);
					}
				}
				break;
			case 4:
				if (rawName.compareToIgnoreCase("a") == 0) {
					Link            l = new Link();
					l.name = getNamedAttribute("name", attributes);
					l.from = getNamedAttribute("from", attributes);
					l.description = getNamedAttribute("description", attributes);
					current_link.links.add(l);
					break;
				}
				System.out.println("[Parser] Parse Error\nflag:" + flag + "\nUnexpected Tag " + rawName);
				printXMLTrace();
				System.exit(0);
				break;
			default:
				System.out.println("[Parser] Unexpected Internal Value. flag:" + flag);
				printXMLTrace();
				System.exit(0);
				break;
		}
	}
	
	public void     characters(char[] ch, int start, int length) throws SAXException {
		if (flag != 10)
			return;
		
		content = new String(ch, start, length);
		
		if (entity_type == 0) {
			Value           value = (Value) container;
			switch          (content_type) {
				case 1:
					value.name = content;
         // System.out.println("name:"+value.name);
					break;
				case 2:
					value.description = content;
					break;
				default:
					System.out.println("[Parser] Unexpected Internal Value. flag:" + flag + " at characters as entity_type:" + entity_type + " content:" + content_type);
					printXMLTrace();
					System.exit(0);
					break;
			}
		} else {
			Function        func = (Function) container;
			switch (content_type) {
				case 1:
					func.name = content;
          // System.out.println("name:"+func.name);
					break;
				case 2:
					func.ret = content;
					break;
				case 3:
					func.description = content;
					break;
				case 4:
					current_link.name = content;
					flag = 4;
					break;
				default:
					System.out.println("[Parser] Unexpected Internal Value. flag:" + flag + " at characters as entity_type:" + entity_type + " content:" + content_type);
					printXMLTrace();
					System.exit(0);
			}
		}
	}
	
	public void     endElement(String uri, String localName, String rawName) {
		String          str = (String) xml_trace.pop();
		
		if              (str.compareToIgnoreCase(rawName) != 0) {
			System.out.println("[Parser] Wrong Closing Tag, expected: " + str);
			printXMLTrace();
			System.exit(0);
		}
		switch          (flag) {
			case 3:
				current_set.entity.add(container);
				break;
			case 10:
				flag = 3;
				break;
		}
		switch (flag) {
			case 4:
				if (rawName.compareToIgnoreCase("link") == 0)
					flag = 3;
				break;
			case 3:
				if (rawName.compareToIgnoreCase("entity") == 0)
					flag = 2;
				break;
			case 2:
				flag = 1;
				break;
		}
	}
	
	public void     startDocument() {
		System.out.println("[Parser] Document Parsing Started");
	}
	
	public void     endDocument() {
		System.out.println("[Parser] Documentation Parsing Completed");
	}
	
	public static void main(String[] args) {
		DocBuilder      doc = null;
		try {
			doc = new DocBuilder();
			
			System.out.println("[System] Parse");
			System.out.println("[Parser] Parsing DevilDoc.xml");
			try {
				SAXParserFactory factory = SAXParserFactory.newInstance();
				factory.setValidating(false);
				factory.newSAXParser().parse(new File("DevilDoc.xml"), doc);
			}               catch(Exception ex) {
				System.out.println("[Error] Cannot Parse DevilDoc.xml");
				System.out.println(ex);
			}
			
			System.out.println();
			System.out.println("[System] Create Documentation");
			
			doc.work();
			
			System.out.println();
			System.out.println("[System] Create Completed");
			System.out.println("[System] Goodbye");
		} catch(java.lang.StackOverflowError stack) {
			doc.printXMLTrace();
			System.out.println("flag:" + doc.flag);
		}
	}
	
	public final void printXMLTrace() {
		Object          obj;
		try {
			while (true) {
				obj = xml_trace.pop();
				System.out.println(obj);
			}
		}               catch(Exception e) {
		}
	}
	
	private final String getNamedAttribute(String name, Attributes attr) {
		int             max = attr.getLength();
		String          str;
		for             (int ctr = 0; ctr < max; ctr++) {
			if (attr.getQName(ctr).compareToIgnoreCase(name) == 0) {
				return attr.getValue(ctr);
			}
		}
		
		return null;
	}
	
	public final void work() {
		int             max_sets_ctr = sets.size();
		for             (int sets_ctr = 0; sets_ctr < max_sets_ctr; sets_ctr++) {
			current_set = ((Set) sets.get(sets_ctr));
			System.out.println();
			System.out.println("[Build] Creating Set " + current_set.name);
			try {
				File            set_dir = new File(current_set.name);
				set_dir.mkdir();
			}               catch(Exception e) {
				System.out.println("[Error] Can't create folder for set " + current_set.name);
			}
			
			int             max_entity_ctr = current_set.entity.size();
			Object          ovector[] = current_set.entity.toArray();
			Entity          vector[] = new Entity[ovector.length];
			System.arraycopy(ovector, 0, vector, 0, ovector.length);
			
			sort(vector, vector.length - 1);
			
			for (int entity_ctr = 0; entity_ctr < max_entity_ctr; entity_ctr++) {
				ByteArrayOutputStream data_out = new ByteArrayOutputStream();
				Entity          e = vector[entity_ctr];
				String          file = current_set.name + "/" + e.name + ".html";
				String          data = null;
				System.out.println("[Build] Creating " + e.name);
				try {
					PrintStream     out = new PrintStream(new BufferedOutputStream(data_out));
					printHeader(out, e.name);
					out.println("<table width=\"100%\" cols=\"2\" border=\"0\" bgcolor=\"navy\"><tr align=\"left\"><td align=\"left\"><font color=\"white\">DevIL Documentation</font></td><td align=\"right\"><font color=\"white\">");
					try {
						String          link = vector[entity_ctr - 1].name + ".html";
						out.println("<a class=\"color\" href=\"" + link + "\">Previous</a> |");
					} catch(IndexOutOfBoundsException aex) {
						out.println("Previous |");
					}
					out.println("<a class=\"color\" href=\"index.html\">" + current_set.name + "</a> |");
					try {
						String          link = vector[entity_ctr + 1].name + ".html";
						out.println("<a class=\"color\" href=\"" + link + "\">Next</a>");
					} catch(IndexOutOfBoundsException aex) {
						out.println("Next ");
					}
					out.println("</font></td></tr></table><br/><table><tr><td>&nbsp;</td><td>");
					out.println("<p><h3>" + e.name + "</h3></p><table><tr><td>&nbsp;</td><td>");
					
					if (e instanceof Value) {
						out.println("");
						out.println("<p><font size=\"-1\">" + conv(((Value) e).description) + "</font></p>");
						out.println("");
					} else {
						Function f = ((Function) e);
						String str = f.description;
						
						// check for images links in form of [/img:<link>]
						str = conv(str);
						out.println("<p><font size=\"-1\">" + str + "</font></p>");
						
						out.print("<table><tr><td>&nbsp;</td><td><p><pre><code><b>" + f.ret + " " + f.name + "(<br/></b>");
						out.print("&nbsp;&nbsp;");
						int             max = f.param.size();
						if (max != 0) {
							for (int ctr = 0; ctr < max; ctr++) {
								if (ctr != 0)
									out.print(", ");
								Param           p = (Param) f.param.elementAt(ctr);
								out.print("<b>" + p.type + "</b> <i>" + p.name + "</i>");
							}
						} else {
							out.print("<b>ILvoid</b>");
						}
						out.println(" <b>);</b></code></pre></p></dl></p></td></tr></table>");
						
						out.print("<br/><p><h4>Parameters</h4>");
						out.print("<table><tr><td>&nbsp;</td><td><dl>");
						max = f.param.size();
						if (max != 0) {
							for (int ctr = 0; ctr < max; ctr++) {
								if (ctr != 0)
									out.print(" ");
								Param           p = (Param) f.param.elementAt(ctr);
								out.print("<dt><i><font size=\"-1\">" + p.name + "</font></i></font></dt><dd><font size=\"-1\">" + p.description + "</font></dd>");
							}
						} else {
							out.println("<p><font face=\"verdana\" size=\"-1\"><i>None</i></font></p>");
						}
						out.print("</dl></p></td></tr></table>");
					}
					out.print("<br/>");
					
					Vector          l = ((LinkedEntity) e).links;
					int             max = l.size();
					int             ctr = 0;
					for (ctr = 0; ctr < max; ctr++) {
						Links           ee = (Links) l.elementAt(ctr);
						sort(ee.links, ee.links.size());
						out.print("<h4>" + ee.name + "</h4><table><tr><td>&nbsp;</td><td><dl>");
						int             lmax = ee.links.size();
						int             lctr = 0;
						if (ee.verbose.compareToIgnoreCase("true") == 0) {
							for (lctr = 0; lctr < lmax; lctr++) {
								Link            eee = (Link) ee.links.elementAt(lctr);
								if (eee.from != null)
									out.print("<dt><a href=\"../" + eee.from + "/" + eee.name + ".html\"><font size=\"-1\">" + eee.name + "</font></a></dt><dd>");
								else
									out.print("<dt><font size=\"-1\">" + eee.name + "</font></dt><dd>");
								if (eee.description == null)
									out.println("<font size=\"-1\">" + getDescription(eee.from, eee.name) + "</font></dd>");
								else
									out.println("<font size=\"-1\">" + eee.description + "</font></dd>");
							}
							out.println("</dl></td></tr></table></p>");
						} else {
							lmax--;
							for (lctr = 0; lctr < lmax; lctr++) {
								Link            eee = (Link) ee.links.elementAt(lctr);
								if (eee.from != null)
									out.println("<a href=\"../" + eee.from + "/" + eee.name + ".html\"><font size=\"-1\">" + eee.name + "</font></a>, ");
								else
									out.println("<font size=\"-1\">" + eee.name + "</font>, ");
								
							}
							Link            eee = (Link) ee.links.elementAt(lctr);
							if (eee.from != null)
								out.println("<a href=\"../" + eee.from + "/" + eee.name + ".html\"><font size=\"-1\">" + eee.name + "</font></a>, ");
							else
								out.println("<font size=\"-1\">" + eee.name + "</font>, ");
							out.println("</td></tr></table></p>");
						}
					}
					out.println("</td></tr></table>");
					printEnd(out);
					out.flush();
					data = data_out.toString();
				} catch(Exception ex) {
					System.out.println("[Error] can't create value " + e.name);
					ex.printStackTrace();
					System.exit(0);
				}
				dumpFile(file, data);
			}
			
			try {
				ByteArrayOutputStream data_out = new ByteArrayOutputStream();
				PrintStream     out = new PrintStream(new BufferedOutputStream(data_out));
				int             first_type = 0;
				printHeader(out, current_set.name);
				out.print("<table width=\"100%\" border=\"0\" bgcolor=\"navy\"><tr align=\"left\"><td align=\"left\"><font color=\"white\">DevIL Documentation</font></td><td align=\"right\"><a class=\"color\" href=\"../index.html\">Frameworks Index</a></td></tr></table>");
				Entity          e;
				int             max;
				int             ctr;
				for (ctr = 0, max = vector.length; ctr < max; ctr++) {
					e = (Entity) vector[ctr];
					if (e instanceof Function) {
						if (first_type == 0)
							out.print("<h2>Functions</h2><table><tr><td>&nbsp;</td><td>");
						out.print("<a href=\"" + e.name + ".html\">" + e.name + "</a><br/>");
						first_type++;
					}
				}
				if (first_type > 0)
					out.print("</td></tr></table><br/>");
				
				first_type = 0;
				for (ctr = 0; ctr < max; ctr++) {
					e = (Entity) vector[ctr];
					if (e instanceof Value) {
						if (first_type == 0)
							out.print("<h2>Values</h2><table><tr><td>&nbsp;</td><td>");
						out.print("<a href=\"" + e.name + ".html\">" + e.name + "</a><br/>");
						first_type++;
					}
				}
				if (first_type > 0)
					out.print("</td></tr></table><br/>");
				
				printEnd(out);
				out.flush();
				dumpFile(current_set.name + "/index.html", data_out.toString());
			} catch(Exception e) {
				System.out.println("[Error] Can't Create " + current_set.name + " index page");
				System.exit(0);
			}
		}
		
		try {
			ByteArrayOutputStream data_out = new ByteArrayOutputStream();
			PrintStream     out = new PrintStream(new BufferedOutputStream(data_out));
			out.print("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\" \"http://www.w3.org/TR/REC-html40/loose.dtd\"><html><head><title>DevIL Developer Documentation</title></head><body topmargin=\"0\" bgcolor=\"white\"><font face=\"verdana\"><img src=\"images/DevIL.gif\"><br></font>");
			out.print("<table width=\"100%\" border=\"0\" bgcolor=\"navy\"><tr align=\"left\"><td align=\"left\"><font color=\"white\">DevIL Documentation</font></td></tr></table>");
			out.print("<h2>Devil Frameworks</h2><p><table><tr><td>&nbsp;</td><td>");
			
			int             ctr, max;
			for (ctr = 0, max = sets.size(); ctr < max; ctr++) {
				Set             set = (Set) sets.elementAt(ctr);
				out.println("<a href=\"" + set.name + "/index.html\"><b>" + set.name + "</b></a><br/>");
			}
			
			out.print("</td></tr></table></p>");
			printEnd(out);
			out.flush();
			dumpFile("index.html", data_out.toString());
		} catch(Exception e) {
			System.out.println("[Error] Can't Create index page");
			System.exit(0);
		}
	}
	
	private final String getDescription(String from, String name) {
		Enumeration     e = sets.elements();
		while           (e.hasMoreElements()) {
			Set             set = (Set) e.nextElement();
			String          setname = set.name;
			if              (setname.compareToIgnoreCase(from) == 0) {
				Enumeration     ee = set.entity.elements();
				while           (ee.hasMoreElements()) {
					Entity          eee = (Entity) ee.nextElement();
					if              (eee.name.compareToIgnoreCase(name) == 0)
						return eee.description;
				}
			}
		}
		System.err.println("[Error] !! Missing definition description in " + from + ":" + name);
		System.exit(1);
		return null;
	}
	
	private final void printHeader(PrintStream out, String title) {
		out.println("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\" \"http://www.w3.org/TR/REC-html40/loose.dtd\"><html><head><title>DevIL Documentation, " + title + "</title></head><body topmargin=\"0\" bgcolor=\"white\"><font face=\"verdana\"><img src=\"../images/DevIL.gif\"><br></font>");
		out.println("<style type=\"text/css\">");
		out.println("A.color{color: FFFFFF;text-decoration: none;}");
		out.println("</style>");
		out.println();
		out.println();
		out.println();
	}
	
	private final void printEnd(PrintStream out) {
		out.println();
		out.println();
		out.println();
		out.println("<br/></body></html>");
	}
	
	private final static void sort(NamedObject A[], int max) {
		int             k, i;
		NamedObject     P;
		
		for             (k = max - 1; k > 0; k--) {
			for (i = 0; i < k; ++i) {
				if (strcmp(A[i].name, A[i + 1].name) > 0) {
					P = A[i];
					A[i] = A[i + 1];
					A[i + 1] = P;
				}
			}
		}
	}
	
	private final static void sort(Vector A, int max) {
		int             k, i;
		NamedObject     P;
		
		for             (k = max - 1; k > 0; k--) {
			for (i = 0; i < k; ++i) {
				if (strcmp(((NamedObject) A.get(i)).name, ((NamedObject) A.get(i + 1)).name) > 0) {
					P = ((NamedObject) A.get(i));
					A.set(i, A.get(i + 1));
					A.set(i + 1, P);
				}
			}
		}
	}
	
	private final String getStringFromName(String set, String name) {
		String          str = "";
		if              (current_set.name != set) {
			str += "../" + set;
		}
		str += name;
		return str;
	}
	
	private static final void dumpFile(String file, String data) {
		try {
			PrintStream     fout = new PrintStream(new FileOutputStream(file));
			fout.print(data);
			fout.close();
			fout = null;
		} catch(Exception ex) {
			System.out.println("[Error] Can't Create File:" + file);
			System.out.println(ex);
		}
	}
	
	private static final int strcmp(String src1, String src2) {
		return src1.compareTo(src2);
	}
	
	//@TODO modifica il testo secondo i link
	// two formats
	// [/link:<GROUP>:<NAME>] 	--> [/link:IL:ilBindImage]
	// [/extlink:<LINK>] 		--> [/extlink:http://www.foo.com]
	public static String conv( String str ) {
		try {
			int idx = 0;
			
			// ogni vettore e' un vettore
			// 1 -> string to match
			// 2 -> pre param
			// 3 -> post param
			String tags[][] =
			{{"%link:","<a href=\"","\" />"},
			 {"%br","</br>",""},
			 {"%center/","</center>",""}, 
			 {"%center","<center>",""},
			 {"%img:","<img src=\"","\" />"}};

			for( int ctr = 0; ctr < tags.length; ctr++ ) {
				int ret = 0;
				while( (ret = find(str,ret,tags[ctr][0])) != -1 ) {
					int end_idx = find(str,ret+tags[ctr][0].length()-2,"%");
					String pre = str.substring(0,ret);
					String post = str.substring(end_idx+1,str.length());
					str = pre + tags[ctr][1] +str.substring(ret+(tags[ctr][0]).length(),end_idx) + tags[ctr][2] + post;
					ret = end_idx;
				}
			}
		} catch( Exception e ) {
			System.err.println("Error while elaborating a tag");
			e.printStackTrace();
			System.exit(-1);
		}
		return str;
	}
	
	
	// Find utility functions
	public static final int find( String str, String param ) {
		return find( str, 0, param );
    }

    public static final int find( String str, int start_from, String param ) {
		int idxs = start_from; // index of the string
		int idxp = 0; // index of the param

		final int slen = str.length(); // length of the string
		final int plen = param.length(); // length of the parameter

		while( idxs < slen && idxp < plen ) { // scan the strings
			if( (idxp < plen) && (param.charAt(idxp) == str.charAt(idxs)) ) // check
				idxp++;
	    		else
				idxp = 0; // reset parameter counter
	    		idxs++;
		}
	
		if( idxp == plen ) // select if not found or found return value
	   		return idxs - plen;
		else
	   		return -1;
    }
}
