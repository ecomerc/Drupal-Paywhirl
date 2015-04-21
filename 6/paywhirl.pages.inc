<?php
	include_once("paywhirl.user.inc");
	
	
	
	function paywhirl_user_page($account) {
		drupal_set_title($account->name);
		$key = variable_get('paywhirl_widgetkey',  ""));
		
		$content = "<script type='text/javascript' src='https://stage.paywhirl.com/js/widget.js'></script>
		<iframe frameBorder='0' style='width:100%; height:auto; border:0; overflow:hidden;' id='paywhirl_frame' scrolling='no' src='https://stage.paywhirl.com/widget/login?api_key=".$key."'></iframe>";
		
		$content = "<div class=\"node\"><div class=\"content\">" . $content . "</div></div>";
		return  $content;
	}
	
	
	function paywhirl_login() {
		$key = variable_get('paywhirl_widgetkey',  ""));
		
		$content = "<script type='text/javascript' src='https://stage.paywhirl.com/js/widget.js'></script>
		<iframe frameBorder='0' style='width:100%; height:auto; border:0; overflow:hidden;' id='paywhirl_frame' scrolling='no' src='https://stage.paywhirl.com/widget/login?api_key=".$key."'></iframe>";
		
		$content = "<div class=\"node\"><div class=\"content\">" . $content . "</div></div>";
		return  $content;
	}
	
	function paywhirl_forgot() {
		$key = variable_get('paywhirl_widgetkey',  ""));
		
		$content = "<script type='text/javascript' src='https://stage.paywhirl.com/js/widget.js'></script>
		<iframe frameBorder='0' style='width:100%; height:auto; border:0; overflow:hidden;' id='paywhirl_frame' scrolling='no' src='https://stage.paywhirl.com/widget/forgot-password?api_key=".$key."'></iframe>";
		
		$content = "<div class=\"node\"><div class=\"content\">" . $content . "</div></div>";
		return  $content;
	}
	
	
	function paywhirl_logout() {	
		header("Location: https://stage.paywhirl.com/widget/logout");
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
				user_logout();
			}
		}
		header("Location: $base_url");
		die();
	}
	
	/*
		
		// Define the errors.
		$constants = get_defined_constants(true);
		$json_errors = array();
		foreach ($constants["json"] as $name => $value) {
		if (!strncmp($name, "JSON_ERROR_", 11)) {
		$json_errors[$value] = $name;
		}
		}
		echo print_r(json_decode($json))."\n";
		echo 'Last error: ', $json_errors[json_last_error()], PHP_EOL, PHP_EOL;
		
	*/
	
	function paywhirl_webhook() {
		
		$key = variable_get('paywhirl_key',  md5("paywhirl" . time ()));
		$args = func_get_args();
		
		
		if ($args[0] == $key) {
			
			// Retrieve the request's body and parse it as JSON
			$input = @file_get_contents("php://input");
			//$event_json = json_decode($input);
			// Do something with $event_json
			http_response_code(200); // PHP 5.4 or greater
			
			// Create our message variables.
			$variables = array('msg' => 'foo');
			
			// Make sure at least one module implements our hook.
			if (sizeof(module_implements('paywhirl_webhook')) > 0) {
				// Call all modules that implement the hook, and let them make changes to $variables.
				$variables = module_invoke_all('paywhirl_webhook', $variables);
			}
			
		}
	}			