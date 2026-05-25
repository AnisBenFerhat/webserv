function setLoading(prefix, loading) {
	document.getElementById(prefix + '-btn').disabled = loading;
}

function statusText(code) {
	var map = {
		200: 'OK', 201: 'Created', 204: 'No Content',
		400: 'Bad Request', 403: 'Forbidden', 404: 'Not Found',
		413: 'Payload Too Large', 500: 'Internal Server Error'
	};
	return map[code] || '';
}

function showResponse(prefix, status, body) {
	var responseElement = document.getElementById(prefix + '-response');
	var responseClass = (status >= 200 && status < 300) ? 'r-ok' : 'r-err';
	responseElement.innerHTML =
		'<span class="' + responseClass + '">HTTP/1.1 ' + status + ' ' + statusText(status) + '</span>\n' +
		(body ? body.substring(0, 300) + (body.length > 300 ? '\n...(truncated)' : '') : '');
	responseElement.classList.add('visible');
}

function sendGet() {
	var path = document.getElementById('get-path').value || '/';
	setLoading('get', true);
	fetch(path)
		.then(function (res) {
			return res.text().then(function (body) {
				showResponse('get', res.status, body);
			});
		})
		.catch(function (err) { showResponse('get', 0, err.toString()); })
		.then(function () { setLoading('get', false); });
}

function sendPost() {
	var file = document.getElementById('post-file').files[0];
	if (!file) { showResponse('post', 0, 'Please select a file.'); return; }

	setLoading('post', true);

	var formData = new FormData();
	formData.append('file', file, file.name);

	fetch('/upload/' + file.name, { method: 'POST', body: formData })
		.then(function (res) {
			return res.text().then(function (body) {
				showResponse('post', res.status, body || '(empty body)');
			});
		})
		.catch(function (err) { showResponse('post', 0, err.toString()); })
		.then(function () { setLoading('post', false); });
}

function sendDelete() {
	var path = document.getElementById('del-path').value;
	if (!path) { showResponse('del', 0, 'Please enter a path.'); return; }

	setLoading('del', true);

	fetch(path, { method: 'DELETE' })
		.then(function (res) {
			return res.text().then(function (body) {
				showResponse('del', res.status, body || '(no content)');
			});
		})
		.catch(function (err) { showResponse('del', 0, err.toString()); })
		.then(function () { setLoading('del', false); });
}
