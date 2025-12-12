from flask import Flask, render_template, request
import subprocess

app = Flask(__name__)

@app.route("/")
def index():
    return render_template("index.html")


@app.route("/run", methods=["POST"])
def run():
    tool = request.form.get("tool")
    target = request.form.get("target")

    cmd_map = {
        "ping": "../network_tool/ping",
        "traceroute": "../network_tool/traceroute",
    }

    cmd = [cmd_map[tool], target]

    try:
        result = subprocess.check_output(cmd, stderr=subprocess.STDOUT, text=True,timeout=5)
    except subprocess.CalledProcessError as e:
        result = e.output

    return render_template("result.html", result=result)


@app.route("/login")
def login():
    return render_template("login.html")


if __name__ == "__main__":

    app.run(
        host="0.0.0.0",
        port=5000,
        debug=True
    )
