<?php
	include_once("paywhirl.user.inc");
	
	
	function paywhirl_breakout($account="") {
		global $user, $base_url;	
		die("<html><head><script>
this.top.location = '$base_url';
</script></head><body><a href='$base_url' target='_top'>If you are not automatically redirected, please click here</a></body></html>");
	}
	
	function paywhirl_user_page($account="") {
		$key = variable_get('paywhirl_widgetkey',  "");
		$server = variable_get('paywhirl_server',  "");
		
		$content = "<p>If you signup for a new plan below and you do not already have a subscription account, please be sure to use the same email address as you are currently logged in with, this will ensure that your accounts are linked, and you will be able to login with your subscription account.</p><script type='text/javascript' src='https://stage.paywhirl.com/js/widget.js'></script>
		<iframe frameBorder='0' style='width:100%; height:auto; border:0; overflow:hidden;' id='paywhirl_frame' scrolling='no' src='$server/widget/login?api_key=".$key."'></iframe>";
		
		$content = "<div class=\"node\"><div class=\"content\">" . $content . "</div></div>";
		return  $content;
	}
	
	function paywhirl_subscribe() {
		$key = variable_get('paywhirl_widgetkey',  "");
		$server = variable_get('paywhirl_server',  "");
		
		$content = "<script type='text/javascript' src='https://stage.paywhirl.com/js/widget.js'></script>
		<iframe frameBorder='0' style='width:100%; height:auto; border:0; overflow:hidden;' id='paywhirl_frame' scrolling='no' src='$server/widget?api_key=".$key."'></iframe>";
		
		$content = "<div class=\"node\"><div class=\"content\">" . $content . "</div></div>";
		return  $content;
	}
	
	function paywhirl_login() {
		$key = variable_get('paywhirl_widgetkey',  "");
		$server = variable_get('paywhirl_server',  "");
		
		//TODO make some sort of setting for the text
		$content = "<p>This is the new subscription based login, if your account was created <b>before</b> April 21st 2015 - <a href='/user'>login here</a></p><script type='text/javascript' src='https://stage.paywhirl.com/js/widget.js'></script>
		<iframe frameBorder='0' style='width:100%; height:auto; border:0; overflow:hidden;' id='paywhirl_frame' scrolling='no' src='$server/widget/login?api_key=".$key."'></iframe>";
		
		$content = "<div class=\"node\"><div class=\"content\">" . $content . "</div></div>";
		return  $content;
	}
	
	function paywhirl_forgot() {
		$key = variable_get('paywhirl_widgetkey',  "");
		$server = variable_get('paywhirl_server',  "");
		
		$content = "<script type='text/javascript' src='https://stage.paywhirl.com/js/widget.js'></script>
		<iframe frameBorder='0' style='width:100%; height:auto; border:0; overflow:hidden;' id='paywhirl_frame' scrolling='no' src='$server/widget/forgot-password?api_key=".$key."'></iframe>";
		
		$content = "<div class=\"node\"><div class=\"content\">" . $content . "</div></div>";
		return  $content;
	}
	
	
	function paywhirl_logout() {	
		$server = variable_get('paywhirl_server',  "");
		
		header("Location: $server/widget/logout");
		die();
	}
	
	function paywhirl_passthrough() {
		global $user, $base_url;	
		
		$key = variable_get('paywhirl_key',  md5("paywhirl" . time ()));
		$args = func_get_args();
		
		
		if ($args[0] == $key) {
			if (isset($_GET["data"])) {
				$serialized = base64_decode($_GET["data"]);
				$data = unserialize($serialized); 
				if (isset($data["stripe_id"])) {
					// PAYWHIRL CREATED USER
					
					$profile = json_decode($data["profile"], true);
					$user = _paywhirl_create_user($data);
					
					} else {
					// PAYWHIRL LOGGING IN
					if ($existing = _paywhirl_load_user($data["email"], $data["password"])) {
						$user = $existing;
					}
					
				}
				} else {
				//LOGOUT
				require_once(drupal_get_path('module', 'user') . '/user.pages.inc');
				$_REQUEST ['destination'] = "$base_url/paywhirl/breakout";
				user_logout();
			}
		}
		header("Location: $base_url/paywhirl/breakout");
		die();
	}
		
	function paywhirl_webhook() {
		
		$key = variable_get('paywhirl_key',  md5("paywhirl" . time ()));
		$args = func_get_args();
		
		
		if ($args[0] == $key) {
			
			// Retrieve the request's body and parse it as JSON
			$input = @file_get_contents("php://input");
			$event_json = json_decode($input);
			
			// Create our message variables.
			$variables = array('data' => $event_json);
			
			// Make sure at least one module implements our hook.
			if (sizeof(module_implements('paywhirl_webhook')) > 0) {
				$variables = module_invoke_all('paywhirl_webhook', $variables);
			}
			
			$dest = file_directory_temp();
			file_save_data($event_json, file_create_path($dest . '/paywhirl.json'), FILE_EXISTS_RENAME);
			
		}
		die("");
	}			