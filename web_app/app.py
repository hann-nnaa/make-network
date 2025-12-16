from flask import Flask, render_template, request
import subprocess
import os

app = Flask(__name__)


# 実行ファイルの絶対パス設定
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
NETWORK_TOOL_DIR = os.path.abspath(
    os.path.join(BASE_DIR, "..", "tool_program")
)

CMD_MAP = {
    "ping": os.path.join(NETWORK_TOOL_DIR, "ping"),
    "traceroute": os.path.join(NETWORK_TOOL_DIR, "traceroute"),
}


# トップページ
@app.route("/")
def index():
    return render_template("index.html")

# コマンド実行
@app.route("/run", methods=["POST"])
def run():
    tool = request.form.get("tool")
    target = request.form.get("target")

    # 入力チェック
    if tool not in CMD_MAP:
        return "Invalid tool"

    if not target:
        return "Target is required"

    cmd_path = CMD_MAP[tool]

    # 実行ファイル存在チェック
    if not os.path.exists(cmd_path):
        return f"Command not found: {cmd_path}"

    if not os.access(cmd_path, os.X_OK):
        return f"Command not executable: {cmd_path}"

    cmd = [cmd_path, target]

    try:
        result = subprocess.check_output(
            cmd,
            stderr=subprocess.STDOUT,
            text=True,
            timeout=5
        )
    except subprocess.TimeoutExpired:
        result = "Error: command timed out"
    except subprocess.CalledProcessError as e:
        result = e.output
    except Exception as e:
        result = f"Unexpected error: {e}"

    return render_template("result.html", result=result)

# ログイン（ダミー）
@app.route("/login")
def login():
    return render_template("login.html")

# Flask 起動
if __name__ == "__main__":
    app.run(
        host="0.0.0.0",
        port=5000,
        debug=True
    )
