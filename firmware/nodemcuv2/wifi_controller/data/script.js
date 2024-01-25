function send_feed_response() {
    const FEED_AMOUNT = document.getElementById('feed_amount').value;
    fetch(`/feed?amount=${FEED_AMOUNT}`)
        .then(response => response.text())
        .then(data => console.log("Response from /feed:", data))
        .catch(error => console.error("Error:", error));
}
