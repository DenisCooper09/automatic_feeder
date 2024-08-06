window.onload = function () {
    fetch('/schedule.json')
        .then(response => response.json())
        .then(data => {
            const list = document.getElementById('feeding_schedule_list');
            list.innerHTML = '';

            data.forEach(item => {
                const li = document.createElement('li');
                li.innerHTML = `
                        <i class="fas fa-arrows-alt"></i>
                        <input type="time" value="${item.time.hour.padStart(2, '0')}:${item.time.minute.padStart(2, '0')}">
                        <input type="number" value="${item.amount}" placeholder="Feed amount (in grams)">
                        <button class="li_button" onclick="remove_feeding_schedule(this)">Delete</button>
                    `;
                list.appendChild(li);
            });
        })
        .catch(error => console.error('Error fetching JSON:', error));
};

function send_request(url, amount, time) {
    fetch(`${url}?amount=${amount}&time=${time}`)
        .then(response => response.text())
        .then(data => console.log(`Response from ${url}:`, data))
        .catch(error => console.error("Error:", error));
}

function feed() {
    const FEED_AMOUNT = document.getElementById('feed_amount').value;
    const CURRENT_TIME = new Date().toISOString();

    send_request('/feed', FEED_AMOUNT, CURRENT_TIME);

    alert("Feeding your cat)");
}

function send_feeding_schedule() {
    const data = get_data_from_list();

    fetch('/schedule', {
        method: 'POST', body: JSON.stringify(data)
    })
        .then(response => response.text())
        .then(data => console.log(`Response from /schedule: `, data))
        .catch(error => console.error("Error: ", error));

    alert("Feeding schedule has been updated!");
}

function add_feeding_schedule() {
    const list = document.getElementById("feeding_schedule_list");
    const li = document.createElement('li');

    li.innerHTML = `
    <i class="fas fa-arrows-alt"></i>
    <input type="time">
    <input type="number" placeholder="grams...">
    <button class="li_button" onclick="remove_feeding_schedule(this)">Delete</button>
    `;

    list.appendChild(li);
}

function remove_feeding_schedule(element) {
    const listItem = element.parentNode;

    listItem.classList.add('removing');

    setTimeout(() => {
        listItem.remove();
    }, 250);
}

function get_data_from_list() {
    const list = document.getElementById("feeding_schedule_list");
    const items = list.getElementsByTagName("li");

    const data = [];

    for (let i = 0; i < items.length; i++) {
        const item = items[i];

        const amount_value = item.querySelector("input[type='number']").value;

        const time_input = item.querySelector("input[type='time']").value.split(':');
        const time_hour = time_input[0];
        const time_minute = time_input[1];

        data.push({
            amount: amount_value, time: {hour: time_hour, minute: time_minute}
        });
    }

    return data;
}

document.addEventListener("DOMContentLoaded", function () {
    Sortable.create(document.getElementById('feeding_schedule_list'), {
        animation: 250, handle: 'i',
    });
});
