let requestCount = 0;
let deletedCount = 0;
let errorCount = 0;
let fileCount = 0;

function checkSession() {
	fetch('/cgi-bin/check_session.py')
		.then(function (res) { return res.json(); })
		.then(function (data) {
			if (!data.valid) {
				window.location.href = '/index.html';
			} else {
				document.getElementById('nav-user').textContent = data.username;
			}
		})
		.catch(function () {
			window.location.href = '/index.html';
		});
}

checkSession();


// Utils

function getCurrentTime() {
	const date = new Date();
	return date.getHours().toString().padStart(2, '0') + ':' +
		date.getMinutes().toString().padStart(2, '0') + ':' +
		date.getSeconds().toString().padStart(2, '0');
}

function formatFileSize(bytes) {
	if (bytes < 1024) return bytes + ' B';
	if (bytes < 1024 * 1024) return Math.round(bytes / 1024) + ' KB';
	return (bytes / 1024 / 1024).toFixed(1) + ' MB';
}

function showToast(message, type) {
	const toast = document.getElementById('toast');
	toast.textContent = message;
	toast.className = 'toast show ' + (type || '');
	setTimeout(function () { toast.className = 'toast'; }, 2800);
}

// Stats bar

function updateStats(count) {
	fileCount = count;
	document.getElementById('stat-files').textContent = fileCount;
	document.getElementById('stat-requests').textContent = requestCount;
	document.getElementById('stat-deleted').textContent = deletedCount;
	document.getElementById('stat-errors').textContent = errorCount;
}

// Request log

function logRequest(method, path, status, durationMs) {
	requestCount++;
	if (status === 0 || status >= 400) errorCount++;

	updateStats(fileCount);

	const emptyMessage = document.getElementById('log-empty');
	if (emptyMessage) emptyMessage.style.display = 'none';

	const methodClass = method === 'GET' ? 'log-method-get' :
		method === 'POST' ? 'log-method-post' : 'log-method-delete';
	const statusClass = status >= 200 && status < 300 ? 'log-status-2xx' :
		status >= 400 ? 'log-status-4xx' : 'log-status-5xx';

	const logItem = document.createElement('div');
	logItem.className = 'log-item';
	logItem.innerHTML =
		'<span class="log-time">' + getCurrentTime() + '</span>' +
		'<span class="log-method ' + methodClass + '">' + method + '</span>' +
		'<span class="log-path">' + path + '</span>' +
		'<span class="log-status ' + statusClass + '">' + status + '</span>' +
		'<span class="log-duration">' + durationMs + 'ms</span>';

	const logList = document.getElementById('log-list');
	logList.insertBefore(logItem, logList.firstChild);
}

// Files list
// TODO - Will be populated from CGI at Ticket 29

function renderFiles(files) {
	const emptyState = document.getElementById('empty-state');
	const filesTable = document.getElementById('files-table');
	const filesBody = document.getElementById('files-body');

	updateStats(files.length);

	if (files.length === 0) {
		emptyState.style.display = 'block';
		filesTable.style.display = 'none';
		return;
	}

	emptyState.style.display = 'none';
	filesTable.style.display = 'table';
	filesBody.innerHTML = '';

	files.forEach(function (file) {
		const row = document.createElement('tr');
		row.id = 'row-' + file.name;
		row.innerHTML =
			'<td>' + file.name + '</td>' +
			'<td class="file-size">' + formatFileSize(file.size) + '</td>' +
			'<td><button class="delete-btn" ' +
			'onclick="deleteFile(\'' + file.name + '\')">DELETE</button></td>';
		filesBody.appendChild(row);
	});
}

function refreshFiles() {
	fetch('/cgi-bin/list_files.py')
		.then(function (res) { return res.json(); })
		.then(function (data) { renderFiles(data.files); })
		.catch(function () {
			// TODO: Will be handled dynamically with the CGI Ticket 29
		});
}

setInterval(refreshFiles, 5000);
refreshFiles();

// Upload

function uploadFile(file) {
	if (!file) return;

	const progressBar = document.getElementById('upload-progress');
	const progressFill = document.getElementById('progress-fill');
	const progressText = document.getElementById('progress-text');

	progressBar.style.display = 'block';
	progressFill.style.width = '30%';
	progressText.textContent = 'Uploading ' + file.name + '...';

	const uploadPath = '/upload/' + file.name;
	const startTime = Date.now();
	const formData = new FormData();
	formData.append('file', file, file.name);

	fetch(uploadPath, { method: 'POST', body: formData })
		.then(function (res) {
			progressFill.style.width = '100%';
			logRequest('POST', uploadPath, res.status, Date.now() - startTime);

			if (res.status === 201 || res.status === 200) {
				showToast(file.name + ' uploaded successfully.', 'success');
				progressText.textContent = 'Done.';
				refreshFiles();
			} else {
				showToast('Upload failed — ' + res.status, 'error');
				progressText.textContent = 'Upload failed (' + res.status + ')';
			}

			setTimeout(function () {
				progressBar.style.display = 'none';
				progressFill.style.width = '0%';
			}, 1800);
		})
		.catch(function () {
			logRequest('POST', uploadPath, 0, Date.now() - startTime);
			showToast('Network error during upload.', 'error');
			progressBar.style.display = 'none';
		});
}

// Delete

function deleteFile(filename) {
	const deletePath = '/upload/' + filename;
	const startTime = Date.now();

	fetch(deletePath, { method: 'DELETE' })
		.then(function (res) {
			logRequest('DELETE', deletePath, res.status, Date.now() - startTime);

			if (res.status === 204 || res.status === 200) {
				deletedCount++;
				showToast(filename + ' deleted.', 'success');
				refreshFiles();
			} else {
				showToast('Delete failed — ' + res.status, 'error');
			}
		})
		.catch(function () {
			logRequest('DELETE', deletePath, 0, Date.now() - startTime);
			showToast('Network error during delete.', 'error');
		});
}

// Drag and drop

function onDragOver(event) {
	event.preventDefault();
	document.getElementById('upload-zone').classList.add('drag-over');
}

function onDragLeave() {
	document.getElementById('upload-zone').classList.remove('drag-over');
}

function onDrop(event) {
	event.preventDefault();
	document.getElementById('upload-zone').classList.remove('drag-over');
	const droppedFile = event.dataTransfer.files[0];
	if (droppedFile) uploadFile(droppedFile);
}
