import java.io.*;
import java.io.RandomAccessFile;
class Reader extends Thread{
      
    public Reader(){
	System.out.println("Reader initialized");
    }

    public void run(){
	/*
	int ch;
	RandomAccessFile fin=null;
	try{
	    fin= new RandomAccessFile("/dev/simplechar","rw");
	    while(true){
		try{
		    while((ch=fin.read())!=-1){
			System.out.println((char) ch);
			//Thread.sleep(50);
		    }	
		}catch (Exception e){System.out.println(e);}
	    }
	} catch(FileNotFoundException e){
	    System.out.println("Ooops! didn't find device");
	    }
	*/
	while(true){
	    try{
		Runtime.getRuntime().exec("cat /dev/simplechar ");
		Runtime.getRuntime().exec("echo 'ddddd'>/dev/simplechar");
	    }catch(Exception e){
		System.out.println(e);
	    }
	}
    }


    public static void main(String[] args){
	Reader r1= new Reader();
	Reader r2= new Reader();
	r1.start();
	r2.start();
    }
}
