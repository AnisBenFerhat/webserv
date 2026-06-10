function setLoading(prefix, loading) {
	document.getElementById(prefix + '-btn').disabled = loading;
}

function statusText(code) {
	const map = {
		200: 'OK', 201: 'Created', 204: 'No Content',
		400: 'Bad Request', 403: 'Forbidden', 404: 'Not Found',
		413: 'Payload Too Large', 500: 'Internal Server Error'
	};
	return map[code] || '';
}

function showResponse(prefix, status, body) {
	const responseElement = document.getElementById(prefix + '-response');
	const responseClass = (status >= 200 && status < 300) ? 'r-ok' : 'r-err';
	responseElement.innerHTML =
		'<span class="' + responseClass + '">HTTP/1.1 ' + status + ' ' + statusText(status) + '</span>\n' +
		(body ? body.substring(0, 300) + (body.length > 300 ? '\n...(truncated)' : '') : '');
	responseElement.classList.add('visible');
}

function sendGet() {
	const path = document.getElementById('get-path').value || '/';
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
	const file = document.getElementById('post-file').files[0];
	if (!file) { showResponse('post', 0, 'Please select a file.'); return; }

	setLoading('post', true);

	const formData = new FormData();
	formData.append('file', file, file.name);

	fetch('/uploads/' + file.name, { method: 'POST', body: formData })
		.then(function (res) {
			return res.text().then(function (body) {
				showResponse('post', res.status, body || '(empty body)');
			});
		})
		.catch(function (err) { showResponse('post', 0, err.toString()); })
		.then(function () { setLoading('post', false); });
}

function sendDelete() {
	const path = document.getElementById('del-path').value;
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

function openLoginModal(event) {
	if (event)
		event.preventDefault();
	const modal = document.getElementById('login-modal');
	modal.style.display = 'flex';
	setTimeout(function () {
		modal.classList.add('show');
	}, 10);
}

function closeLoginModal() {
	const modal = document.getElementById('login-modal');
	modal.classList.remove('show');
	setTimeout(function () {
		modal.style.display = 'none';
	}, 200);
}

if (window.location.search.indexOf('login_error=1') !== -1) {
	openLoginModal();
	document.getElementById('login-error-msg').classList.add('show');
	window.history.replaceState({}, document.title, window.location.pathname);
}
