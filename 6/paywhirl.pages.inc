<?php
	include_once("paywhirl.user.inc");

	
	function _paywhirl_create_user($paywhirldata) {
		
		if ($existing = user_load(array('mail' => $paywhirldata["email"]))) {
		
			$account['paywhirl_id'] = $paywhirldata["stripe_id"];
			$account['paywhirl_password'] = md5("" . $paywhirldata["password"]);
		
			$existing = user_save($existing, $account);
			$password = "";
			return $existing;
		}
		else {
			// Only set a user name if we are not updating an existing record.
			if (strpos($paywhirldata["email"], "@") > 1) {
				$username = drupal_substr($paywhirldata["email"], 0, strpos($paywhirldata["email"], "@"));
			} else {
				$username = $paywhirldata["first_name"] . $paywhirldata["last_name"];
			}
			$account = array();
			$account['name'] = _paywhirl_create_username(trim($username));
			$account['mail'] = $paywhirldata["email"];
			$account['profile_name'] = $paywhirldata["first_name"] . " " . $paywhirldata["last_name"];
			$account['pass'] = $paywhirldata["password"];
			$account['access'] = 1;
			$account['status'] = 1;			
			$account['paywhirl_id'] = $paywhirldata["stripe_id"];
			$account['paywhirl_password'] = md5("" . $paywhirldata["password"]);
			
			$profile = json_decode($data["profile"], true);
			foreach (array_expression as $key => $value) {
				$account["profile_".strtolower($key)] = $value;
			}
			
			
			$account = user_save('', $account);
			return $account;
		}
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
						print_r($existing);
						die();
						$user = $existing;
					}
					
				}
				print_r($data);
				die("done");
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