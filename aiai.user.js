// ==UserScript==
// @match https://app.aiai.se/tidrapport/*
// @version 1.0.0
// ==/UserScript==

const TAX = (1 - 0.225);
const HOURLY_RATE = 0;
const OB_RATE = HOURLY_RATE + 0;
const HOLIDAY_RATE = HOURLY_RATE + 0;

/* Total hours, and the element to inject the pay into */
let hours = document.getElementById("sumColumnT_bru118167ass120887");
let inject = document.getElementById("sumColumnVK_bru118167ass120887");

/* total hours worked, OB AND HOLIDAY NEEDS TO BE COUNTED SEPARATELY */
let time = time_to_float(hours.innerText);

/* traverse through table's rows, count OB (mornings, before 7), and holidays (column 8 (index 7)) */
let table = document.getElementById("bru118167ass120887");
let hours_ob = 0;
let hours_holiday = 0;
for (let i = 1; i < table.rows.length; i++) {
    row = table.rows[i];

    let start_hr = time_to_float(row.cells[1].innerText);
    if (start_hr < 8)
        hours_ob += 8 - start_hr;

    /* storhelg finns i kolumn 8 (ie 7), äre tomt så äre tomt */
    let holiday = row.cells[7].innerText;
    if (holiday == ' ')
        continue;
    console.log("Holiday: '" + holiday + "'");
    hours_holiday += time_to_float(holiday);
}

/* calculate the regular pay, tax it, then ob, tax it, and finally holiday and tax it */
let pay = Math.round((time) * (HOURLY_RATE * TAX)
                      + hours_ob * (OB_RATE * TAX)
		      + hours_holiday * (HOLIDAY_RATE * TAX));

let pay_hr = pay.toString().replace(/\B(?=(\d{3})+(?!\d))/g, " ");

inject.innerText = "~" + pay_hr + " kr";

function time_to_float(str) {
    let x = str.split(":");
    if (x.length == 1)
        return parseInt(x[0]);
	return parseInt(x[0]) + parseFloat(x[1] / 60);
}
