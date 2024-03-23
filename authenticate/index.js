import express from "express";
import bodyParser from "body-parser";
import mysql from "mysql2";

const app = express();
const port = 3000;

const db_w = mysql.createConnection({
  host: "localhost",
  user: "root",
  password: "Password",
  database: "clinical_flex"
});

app.use(bodyParser.urlencoded({ extended: true }));
app.use(express.static("public"));

app.get("/", (req, res) => {
  res.render("home.ejs");
});

app.get("/login", (req, res) => {
  res.render("login.ejs");
});

app.get("/register", (req, res) => {
  res.render("register.ejs");
});

app.post("/register", async (req, res) => {
  const email = req.body.username;
  const password = req.body.password;
  try {
    db_w.connect(function(err) { if (err) throw err;
      var sql = "SELECT * FROM users WHERE email = \"" + email + "\"";
      db_w.query(sql, function (err, result) {
        if (err) throw err;
        if (result.length > 0) {
          res.send("Email already exists. Try logging in.");
          db_w.end();
        } 
        else {
          sql = "INSERT INTO users (email, password) VALUES ( \"" + email + "\",\"" + password + "\")";
          db_w.query(sql, function (err, result) { if (err) throw err; db_w.end();});
          res.render("secrets.ejs");
        }
      });
    });
  } 
  catch (err) {
    console.log(err);
  }
});

app.post("/login", async (req, res) => {
  const email = req.body.username;
  const password = req.body.password;
  console.log(email, password);
  try {
    db_w.connect(function(err) { if (err) throw err;
      var sql = "SELECT * FROM users WHERE email = \"" + email + "\"";
      db_w.query(sql, function (err, result) {
        if (err) throw err;
        if (result.length > 0) {
          const user = result[0];
          const storedPassword = user.password;
          if (password == storedPassword) {
            res.render("secrets.ejs");
          } else {
            res.send("Incorrect Password");
          }
        } else {
          console.log(result);
          res.send("User not found");
        }
      });
  });
  } catch (err) {
    console.log(err);
  }
});

app.listen(port, () => {
  console.log(`Server running on port ${port}`);
});
