<?php
	class IOS
	{	
		function aliasreset()
		{
			$o = array();
			exec( 'sudo ios -v aliasreset',$o);
			return ($o[0] == 'ok' ) ? true : false;
		}
		
		function als()
		{
			$o = array();
			exec( 'sudo ios als',$o);
			return $o;
		}
		
		function ls()
		{
			$o = array();
			exec( 'sudo ios ls',$o);
			return $o;
		}
		
		function isDevice($name)
		{
			$o = $this->ls();
			for ( $i = 0; $i < count($o); ++$i)
				if ( $o == $name ) return true;
			return false;
		}	
		
		function export($pin)
		{
			$o = array();
			exec( 'sudo ios -v export ' . $pin,$o);
			return ($o[0] == 'ok' || $o[0] == 'all ok' ) ? true : false;
		}
		
		function unexport($pin)
		{
			$o = array();
			exec( 'sudo ios -v unexport ' . $pin,$o);
			return ($o[0] == 'ok' || $o[0] == 'all ok' ) ? true : false;
		}
		
		function exported($pin)
		{
			$o = array();
			exec( 'sudo ios -v exported ' . $pin,$o);
			return ($o[0] == '1') ? true : false;
		}
		
		function mdelay($msec)
		{
			exec( 'sudo ios mdelay ' . $msec);
		}
		
		function udelay($usec)
		{
			exec( 'sudo ios udelay ' . $usec);
		}
		
		function ndelay($msec)
		{
			exec( 'sudo ios ndelay ' . $nsec);
		}
		
		function newDevice($name, $typedv, $mode)
		{
			$o = array();
			exec( 'sudo ios -v ' . $name . ' -o ' . $typedv . ' ' . $mode , $o);
			return ($o[0] == '1') ? true : false;
		}
		
		function closeDevice($name)
		{
			$o = array();
			exec( 'sudo ios -v ' . $name . ' -c', $o);
			return ($o[0] == '1') ? true : false;
		}
		
		function writeDevice($name, $val, $ssize = '')
		{
			$o = array();
			exec( 'sudo ios -v ' . $name . ' -w ' . $sz . ' ' . $val, $o);
			return ($o[0] == '1') ? true : false;
		}
		
		function readDevice($name, $ssize = '')
		{
			$o = array();
			exec( 'sudo ios -v ' . $name . ' -r ' . $ssize, $o);
			return $o[0];
		}
		
		function ioctlDevice($name, $req, $val, $ssize = '')
		{
			$o = array();
			exec( 'sudo ios -v ' . $name . ' -i ' . $req . ' ' . $ssize . ' ' .$val, $o);
			return ($o[0] == '1') ? true : false;
		}
		
	}	
	
	
	class PINDEV extends IOS
	{
		protected $name;
		protected $auto;
		
		function __construct ($name, $pin = '', $mode = '', $auto = false)
		{
			if ($pin == '' || $mode == '')
			{
				if ( !$this->isDevice($name) )
					return null;
			}
			else	
			{
				if ( !$this->newDevice($name,'ios/pin',$mode) )
					return null;
				$this->ioctlDevice($name,'pinset',$pin);
			}
			
			$this->name = $name;
			$this->auto = $auto; 
		}
		
		function __destruct()
		{
			if ( $this->auto ) $this->closeDevice($this->name);
		}
		
		function out($val)
		{
			$this->writeDevice($this->name,$val);
		}
		
		function in()
		{
			return $this->readDevice($this->name);
		}
	}
	
	class PORTDEV extends IOS
	{
		protected $name;
		protected $auto;
		
		function __construct ($name, $pinall = array(), $mode = '', $auto = false)
		{
			if ($mode == '')
			{
				if ( !$this->isDevice($name) )
					return null;
			}
			else	
			{
				if ( !$this->newDevice($name,'ios/port',$mode) )
					return null;
				if ( count($pinall) )
				{
					for ($i = 0; $i < count($pinall); ++$i)
						$this->ioctlDevice($name,'portset' . $i,$pinall[$i]);
				}
			}
			
			$this->name = $name;
			$this->auto = $auto; 
		}
		
		function __destruct()
		{
			if ( $this->auto ) $this->closeDevice($this->name);
		}
		
		function pinset($n,$p)
		{
			$this->ioctlDevice($name,'portset' . $n,$p);
		}
		
		function out($val)
		{
			$this->writeDevice($this->name,$val);
		}
		
		function in()
		{
			return $this->readDevice($this->name);
		}
	}
	
	class PULSEDEV extends IOS
	{
		protected $name;
		protected $auto;
		
		function __construct ($name, $pin = '', $mode = '', $auto = false)
		{
			if ($mode == '')
			{
				if ( !$this->isDevice($name) )
					return null;
			}
			else	
			{
				if ( !$this->newDevice($name,'ios/pulse',$mode) )
					return null;
				
				$this->ioctlDevice($name,'pulseset',$pin);
			}
			
			$this->name = $name;
			$this->auto = $auto; 
		}
		
		function __destruct()
		{
			if ( $this->auto ) $this->closeDevice($this->name);
		}
		
		function pulseMode($m)
		{
			$this->ioctlDevice($name,'pulsemode',$m);
		}
		
		function pulseUs($us)
		{
			$this->ioctlDevice($name,'pulseus',$us);
		}
		
		
		function out()
		{
			$this->writeDevice($this->name,'1');
		}
		
		function in()
		{
			//return $this->readDevice($this->name);
		}
	}
?>
