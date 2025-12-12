# Portfolio - Network Tool & Mini Flask App

## 概要
九州工業大学 情報通信工学科 3年 プロジェクトサンプル。

### 含まれるもの
- `network_tool/`: ping, traceroute, simple port-scan を行う Python スクリプト
- `web_app/`: Flask ミニアプリ（登録・ログイン・投稿機能、SQLite）

## 実行方法
### Network tool
```bash
cd network_tool
python3 network_tool.py --host example.com --ping 4 --traceroute --ports 22,80 --out result.txt
