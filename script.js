document.getElementById("loginForm").addEventListener("submit", function(e) {
    e.preventDefault();

    const regNo = document.getElementById("regno").value.trim();
    const name = document.getElementById("name").value.trim();
    const password = document.getElementById("password").value.trim();

    const resultDiv = document.getElementById("result");
    const loginBtn = document.getElementById("loginBtn");
    const btnText = loginBtn.querySelector(".btn-text");
    const spinner = loginBtn.querySelector(".spinner-med");

    resultDiv.style.display = "none";
    resultDiv.className = ""; 
    loginBtn.disabled = true;
    btnText.textContent = "Authenticating...";
    spinner.style.display = "block";

    // Send data to server
    fetch("http://localhost:8080", {
        method: "POST",
        headers: { "Content-Type": "application/x-www-form-urlencoded" },
        body: `regno=${encodeURIComponent(regNo)}&name=${encodeURIComponent(name)}&password=${encodeURIComponent(password)}`
    })
    .then(res => res.text()) 
    .then(data => {
        console.log("Server response:", data);

        if (data.trim() === "success") {
            // ✅ SAVE USER DATA TO BROWSER MEMORY
            localStorage.setItem("isLoggedIn", "true");
            localStorage.setItem("userName", name);
            localStorage.setItem("userRegNo", regNo);

            resultDiv.textContent = "Login Successful! Redirecting...";
            resultDiv.className = "success";
            resultDiv.style.display = "block";
            
            btnText.textContent = "Success";
            spinner.style.display = "none";
            loginBtn.style.backgroundColor = "#10b981"; 

            // Redirect to dashboard
            setTimeout(() => {
                window.location.href = "dashboard.html";
            }, 1000);

        } else {
            throw new Error("Invalid Credentials");
        }
    })
    .catch(err => {
        console.error(err);
        
        resultDiv.style.display = "block";
        
        if (err.message === "Failed to fetch") {
            resultDiv.className = "error";
            resultDiv.innerHTML = `<i class="fa-solid fa-circle-exclamation"></i> Server Error. Is C++ program running?`;
        } else {
            resultDiv.className = "error";
            resultDiv.innerHTML = `<i class="fa-solid fa-circle-xmark"></i> Invalid Credentials.`;
        }

        loginBtn.disabled = false;
        btnText.textContent = "Verify Identity";
        spinner.style.display = "none";
        loginBtn.style.backgroundColor = "var(--primary)";
    });
});