import java.io.*;
class Reader extends Thread{
    
    public Reader(){
	System.out.println("Reader initialized");
    }

    public void run(){
	int ch;
	//File file= new File("/dev/simplechar");
	//FileInputStream fin=null;
	while(true){
	    try{/*
		fin= new FileInputStream(file);
		while((ch=fin.read())!=-1)
		    System.out.println((char) ch);
		    fin.close();*/
		Runtime.getRuntime().exec("cat /dev/simplechar ");
	    }catch (Exception e){ }
	}
    }

    public static void main(String[] args){
	Reader reader1= new Reader();
	Reader reader2= new Reader();
	Reader r3 = new Reader();
	reader1.start();
	reader2.start();
	r3.start();
    }
}
