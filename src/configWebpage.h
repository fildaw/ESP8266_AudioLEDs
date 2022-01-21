#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <eepromIOnew.h>
#include <constDB.h>
#include <ledPulse.h>

const String configWebpageHTML = "<!doctype html>" \
"<html lang='en'>" \
"  <head>" \
"    <meta charset='utf-8'>" \
"    <meta name='viewport' content='width=device-width, initial-scale=1'>" \
"    <link href='/bootstrap.min.css' rel='stylesheet'>" \
"    <title>Config webpage</title>" \
"	<script>" \
"	function resetToDefault() {" \
"		document.getElementById('ip-input').value='192.168.1.150';" \
"		document.getElementById('subnet-input').value='255.255.255.0';" \
"		document.getElementById('gateway-input').value='192.168.1.1';" \
"		document.getElementById('port-input').value='7777';" \
"		document.getElementById('n-leds-input').value='30';" \
"	}" \
"	function validateIP(ip) {" \
"		var splitted = ip.split('.');" \
"		if (splitted.length != 4) {" \
"			return false;" \
"		}" \
"		for (var i = 0; i < 4; i++) {" \
"			var b = parseInt(splitted[i]);" \
"			if (b < 0 || b > 255) {" \
"				return false;" \
"			}" \
"		}" \
"		return true;" \
"	}" \
"	function validateInput() {" \
"		var ssid = document.forms['config-form']['ssid-input'].value;" \
"		var psk = document.forms['config-form']['psk-input'].value;" \
"		var ip = document.forms['config-form']['ip-input'].value;" \
"		var subnet = document.forms['config-form']['subnet-input'].value;" \
"		var gateway = document.forms['config-form']['gateway-input'].value;" \
"		var port = parseInt(document.forms['config-form']['port-input'].value);" \
"		var nLeds = parseInt(document.forms['config-form']['port-input'].value);" \
"		var error = false;" \
"		if (ssid.length > 32) {" \
"			document.forms['config-form']['ssid-input'].classList.add('is-invalid');" \
"			error = true;" \
"		} else {" \
"			document.forms['config-form']['ssid-input'].classList.remove('is-invalid');" \
"		}" \
"		if (psk.length < 8 || psk.length > 63) {" \
"			document.forms['config-form']['psk-input'].classList.add('is-invalid');" \
"			error = true;" \
"		} else {" \
"			document.forms['config-form']['psk-input'].classList.remove('is-invalid');" \
"		}" \
"		if (!validateIP(ip)) {" \
"			document.forms['config-form']['ip-input'].classList.add('is-invalid');" \
"			error = true;" \
"		} else {" \
"			document.forms['config-form']['ip-input'].classList.remove('is-invalid');" \
"		}" \
"		if (!validateIP(subnet)) {" \
"			document.forms['config-form']['subnet-input'].classList.add('is-invalid');" \
"			error = true;" \
"		} else {" \
"			document.forms['config-form']['subnet-input'].classList.remove('is-invalid');" \
"		}" \
"		if (!validateIP(gateway)) {" \
"			document.forms['config-form']['gateway-input'].classList.add('is-invalid');" \
"			error = true;" \
"		} else {" \
"			document.forms['config-form']['gateway-input'].classList.remove('is-invalid');" \
"		}" \
"		if (error) {" \
"			return false;" \
"		}" \
"		return true;" \
"	}" \
"	</script>" \
"	<script>" \
"        function showpass() {" \
"			if (document.getElementById('show-password').checked) {" \
"				document.getElementById('psk-input').type = 'text';" \
"			} else {" \
"				document.getElementById('psk-input').type = 'password';" \
"			}" \
"		}" \
"    </script>" \
"  </head>" \
"  <body>" \
"	<div class='container-fluid'>" \
"    <h1>ESP8266 config</h1>" \
"	<br><br>" \
"    <script src='/bootstrap.min.js'></script>" \
"	<form name='config-form' onsubmit='return validateInput()' method='post' action='/save-config'>" \
"	  <div class='form-group'>" \
"	    <label for='ssid-input'>SSID</label>" \
"	    <input type='text' class='form-control' id='ssid-input' value='$SSID$' name='ssid'/>" \
"		<label for='psk-input'>PSK</label>" \
"		<input type='password' class='form-control' id='psk-input' value='$PSK$' name='psk' />" \
"		<input class='form-check-input' type='checkbox' id='show-password' onclick='showpass();'>" \
"        <label class='form-check-label' for='show-password'>Show password</label>" \
"		<br><br>" \
"	    <label for='ip-input'>IP</label>" \
"	    <input type='text' class='form-control' id='ip-input' value='$IP$' name='ip'/>" \
"	    <label for='subnet-input'>Subnet</label>" \
"	    <input type='text' class='form-control' id='subnet-input' value='$SUBNET$' name='subnet'/>" \
"		<label for='gateway-input'>Gateway</label>" \
"	    <input type='text' class='form-control' id='gateway-input' value='$GATEWAY$' name='gateway'/>" \
"		<label for='port-input'>Port</label>" \
"		<input type='number' class='form-control' id='port-input' value='$PORT$' max='65535' min='1' name='port' />" \
"	  </div>" \
"	  <br>" \
"	  <div class='form-group'>" \
"		<label for='n-leds-input'>Number of leds on led strip</label>" \
"		<input type='number' id='n-leds-input' class='form-control' value='$NLEDS$' max='256' min='3' name='nleds'/>" \
"	  </div>" \
"	  <br>" \
"	  <div class='row'>" \
"        <div class='col-sm-12 text-center'>" \
"          <button type='submit' id='btn-save' class='btn btn-primary btn-md center-block'>Save</button>" \
"          <button id='btn-clear' class='btn btn-danger btn-md center-block' OnClick='resetToDefault()' type='button'>Reset to default</button>" \
"        </div>" \
"      </div>" \
"	</form>" \
"	</div>" \
"  </body>" \
"</html>";

void setupWebserver();
void waitForRequest();
