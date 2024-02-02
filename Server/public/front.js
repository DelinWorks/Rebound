async function sendPostRequest(link, data) {
  const serverUrl = link;

  const requestData = {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(data),
  };

  try {
    const response = await fetch(serverUrl, requestData);
    const responseData = await response.json();
    return { status: response.status, data: responseData };
  } catch (error) {
    return { status: 500, error: "unknown error occurred" }; // Default to a status code of 500 for errors
  }
}

function convertToUppercase(input) {
  input.value = input.value.toUpperCase();
}
