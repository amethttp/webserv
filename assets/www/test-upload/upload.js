const form = document.getElementById("form");
const messageLabel = document.getElementById("messageLabel");

form.onsubmit = (event) => {
	messageLabel.innerHTML = "";
	event.preventDefault();
	console.log(form, event);
	postFile(document.getElementById("inputFile").files[0]);
}

const postFile = (file) => {
	console.log(file);
	fetch(`/${file.name}`, {
		method: 'POST',
		body: file
	})
		.then(response => {
			if (!response.ok) {
				messageLabel.innerHTML = response.status;
				throw new Error(`HTTP error! status: ${response.status}`);
			}
			return response;
		})
		.then(data => {
			console.log('Success: ', data);
			location.href = `/uploads/${file.name}`;
		})
		.catch(error => {
			console.error('Error:', error);
		});
}