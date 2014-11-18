<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1 plus MathML 2.0//EN" "http://www.w3.org/Math/DTD/mathml2/xhtml-math11-f.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<html>
	<head>
		<meta http-equiv="Cache-Control" content="no-cache"/>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
		
		<?php
			session_start();
			
			function printarray($ar)
			{
				$ret = '';
				for ($i = 0; $i < count($ar); ++$i)
					$ret .= $ar[$i] . '<br>'; 
				return $ret;
			}
			
			include ('./phpios.php');
			
			$msg = '';
			$ios = new IOS();
			
			if ( isset($_POST['Actios']) )
			{
				switch ($_POST['Actios'])
				{	
					case 'als':
						$msg = printarray($ios->als()); 
					break;
					
					case 'ls':
						$msg = printarray($ios->ls()); 
					break;
					
					case 'export':
						$msg = ($ios->export($_POST['TxtPinEx'])) ? 'TRUE' : 'FALSE'; 
					break;
					
					case 'unexport':
						$msg = ($ios->unexport($_POST['TxtPinEx'])) ? 'TRUE' : 'FALSE'; 
					break;
					
					case 'exported':
						$msg = ($ios->exported($_POST['TxtPinEx'])) ? 'TRUE' : 'FALSE';
					break;
					
					case 'open':
						$msg = ($ios->newDevice($_POST['TxtDevName'],$_POST['TxtType'],$_POST['TxtMode'])) ? 'TRUE' : 'FALSE';
					break;
					
					case 'write':
						$msg = ($ios->writeDevice($_POST['TxtDevName'],$_POST['TxtValue'])) ? 'TRUE' : 'FALSE';
					break;
					
					case 'read':
						$msg = ($ios->readDevice($_POST['TxtDevName'],$_POST['TxtValue'])) ? 'TRUE' : 'FALSE';
					break;
					
					case 'ioctl':
						$msg = ($ios->ioctlDevice($_POST['TxtDevName'],$_POST['TxtRequest'],$_POST['TxtValue'])) ? 'TRUE' : 'FALSE';
					break;
					
					case 'close':
						$msg = ($ios->closeDevice($_POST['TxtDevName'])) ? 'TRUE' : 'FALSE';
					break;
					
					case 'test':
						$led = new PINDEV('myled',$_POST['TxtPinEx'],'w',true);
						
						$led->out(1);
						$led->mdelay(500);
						$led->out(0);
						$led->mdelay(500);
						$led->out(1);
						$led->mdelay(500);
						$led->out(0);
						$led->mdelay(500);
						$led->out(1);
						$led->mdelay(500);
						$led->out(0);
						
						$led = null;
						
						$pa = array('p2','p3','p4','p5');
						$led = new PORTDEV('alled',$pa,'w',true);
						
						$led->out(0x0F);
						$led->mdelay(500);
						$led->out(0x0A);
						$led->mdelay(500);
						$led->out(0x05);
						$led->mdelay(500);
						$led->out(0x0A);
						$led->mdelay(500);
						$led->out(0x05);
						$led->mdelay(500);
						$led->out(0x0F);
						$led->mdelay(500);
						$led->out(0x00);
						
						$led = NULL;
						
						$led = new PULSEDEV('myled',$_POST['TxtPinEx'],'w',true);
						$led->pulseMode(1);
						$led->pulseUs(1000);
						
						$led->out();
						$led->mdelay(500);
						$led->out();
						
						$led = null;
					break;
				}
			}
		?>
    </head>

    <body>
        <form name = "frmios" method="POST" >
			<h1>IOS</h1>
			<hr/>
			
			<p style="font-size:100%;">
				<?php echo $msg; ?>
			</p>
			<hr/>
				<button type="submit" name ="Actios" value="als">alias list</button>
				<button type="submit" name ="Actios" value="ls">device list</button><br/> 
			<hr/>	
				pin name:<textarea name="TxtPinEx" rows="1" cols="8"></textarea></br>
				<button type="submit" name ="Actios" value="export">export</button>
				<button type="submit" name ="Actios" value="unexport">unexport</button>
				<button type="submit" name ="Actios" value="exported">exported</button>
				<button type="submit" name ="Actios" value="test">test class</button><br/> 
			<hr/>
				device name:<textarea name="TxtDevName" rows="1" cols="16"></textarea></br>
				type:<textarea name="TxtType" rows="1" cols="16"></textarea>
				mode:<textarea name="TxtMode" rows="1" cols="4"></textarea></br>
				request:<textarea name="TxtRequest" rows="1" cols="16"></textarea></br>
				value<textarea name="TxtValue" rows="1" cols="8"></textarea></br>
				<button type="submit" name ="Actios" value="open">new device</button>
				<button type="submit" name ="Actios" value="write">write</button>
				<button type="submit" name ="Actios" value="read">read</button>
				<button type="submit" name ="Actios" value="ioctl">ioctl</button>
				<button type="submit" name ="Actios" value="close">close</button><br/> 
			<hr/>
				<button type="submit" name ="Actios" value="abc">abc</button><br/> 
        </form>
    </body>
</html>
