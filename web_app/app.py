from flask import Flask, render_template, request
from flask_socketio import SocketIO, emit
import subprocess
import os
import re
import time
import random

app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*")


# 実行ファイルの絶対パス設定
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
NETWORK_TOOL_DIR = os.path.abspath(
    os.path.join(BASE_DIR, "..", "tool_program")
)

PING_CMD = os.path.join(NETWORK_TOOL_DIR, "ping")

# クライアントごとの一時停止フラグ（sid -> bool）
measure_paused = {}


@app.route("/")
def index():
    """
     配信をイメージした
    「配信遅延可視化ダッシュボード」画面を返す。
    """
    return render_template("index.html")


def run_ping_once(target_ip: str) -> float | None:
    """
    C 言語で実装した ping 実行ファイルを 1 回だけ実行し、
    出力から RTT(ms) をパースして返す。
    取得できなければ None を返す。
    """
    if not os.path.exists(PING_CMD) or not os.access(PING_CMD, os.X_OK):
        return None

    try:
        # ping <IP> を 1 回だけ実行
        out = subprocess.check_output(
            [PING_CMD, target_ip],
            stderr=subprocess.STDOUT,
            text=True,
            timeout=3,
        )
    except Exception:
        return None

    # 例: "PING 8.8.8.8: time=22.71 ms"
    m = re.search(r"time=([0-9.]+)\s*ms", out)
    if not m:
        return None

    return float(m.group(1))


def latency_stream(target_ip: str, sid: str):
    """
    視聴者 1 人分の「配信遅延(=RTT を簡易モデルとみなす)」を継続計測し、
    WebSocket 経由でフロントに push するバックグラウンドタスク。
    """
    # 簡易的な stop フラグ。切断時に False に更新される。
    # （本気で作り込む場合は redis 等で管理する）
    running = True

    # コメントも一緒に流すため、ここでコメント生成も行う
    last_comment_time = time.time()
    COMMENT_TEXTS = [
        "ナイスプレイ！",
        "コメント読み助かる！",
        "草",
        "今日も配信ありがとう！",
        "ラグちょっと増えた？",
        "低遅延モードいい感じ！",
    ]

    while running:
        # 一時停止中は待機
        while measure_paused.get(sid, False):
            socketio.sleep(0.3)

        start_ts = time.time()
        rtt_ms = run_ping_once(target_ip)

        now = time.time()
        payload = {
            "timestamp": now,
            "rtt_ms": rtt_ms,
        }

        socketio.emit("latency", payload, room=sid)

        # 疑似コメントサーバ: 数秒おきにコメントを push
        if now - last_comment_time > random.uniform(2.0, 5.0):
            last_comment_time = now
            comment = random.choice(COMMENT_TEXTS)
            socketio.emit(
                "comment",
                {
                    "text": comment,
                    # 「コメントサーバが送信した時刻」という想定
                    "sent_at": now,
                },
                room=sid,
            )

        # おおよそ 1 秒間隔になるよう sleep（計測＋コメント送信時間も考慮）
        elapsed = time.time() - start_ts
        wait = max(0.2, 1.0 - elapsed)
        socketio.sleep(wait)


@socketio.on("start_measure")
def handle_start_measure(data):
    """
    フロントからの計測開始要求。
    data: { "target_ip": "8.8.8.8" }
    """
    target_ip = data.get("target_ip")
    if not target_ip:
        emit("error", {"message": "ターゲット IP が指定されていません"})
        return

    sid = request.sid  # type: ignore[name-defined]
    measure_paused[sid] = False
    socketio.start_background_task(latency_stream, target_ip, sid)


@socketio.on("pause_measure")
def handle_pause_measure():
    """フロントからの計測一時停止要求。"""
    sid = request.sid  # type: ignore[name-defined]
    measure_paused[sid] = True


@socketio.on("resume_measure")
def handle_resume_measure():
    """フロントからの計測再開要求。"""
    sid = request.sid  # type: ignore[name-defined]
    measure_paused[sid] = False


@socketio.on("connect")
def handle_connect():
    emit("info", {"message": "配信遅延可視化サーバに接続しました"})


@socketio.on("disconnect")
def handle_disconnect():
    sid = request.sid  # type: ignore[name-defined]
    measure_paused.pop(sid, None)


if __name__ == "__main__":
    # WebSocket を使うため、flask-socketio から起動
    socketio.run(
        app,
        host="0.0.0.0",
        port=5000,
        debug=True,
    )