SHELL=bash

all: a.out

a.out: main.c
	gcc ${CFLAGS} main.c -g -ljansson

run: a.out
	./a.out

test: ./a.out
	-timeout 60 ./a.out &
	@diff <(echo -en '404 Not Found\r\n') <(curl -s -X GET http://localhost:8080/api/v1/event/1)
	@diff <(echo -en '200 OK\r\n{"events": []}' | sed '1d' | python3 -m json.tool) <(curl -s -X GET http://localhost:8080/api/v1/event | sed '1d' | python3 -m json.tool)
	@diff <(echo -en '200 OK\r\n{"status": "success", "message": "registered", "id": 0}' | sed '1d' | python3 -m json.tool) <(curl -s -X POST -H "Content-Type: application/json" -d '{"deadline": "2019-06-11T14:00:00+09:00", "title": "レポート提出", "memo": ""}' http://localhost:8080/api/v1/event | sed '1d' | python3 -m json.tool)
	@diff <(echo -en '400 Bad Request\r\n{"status": "failure", "message": "invalid date format"}' | sed '1d' | python3 -m json.tool) <(curl -s -X POST -H "Content-Type: application/json" -d '{"deadline": "hogehoge", "title": "レポート提出", "memo": ""}' http://localhost:8080/api/v1/event | sed '1d' | python3 -m json.tool)
	@diff <(echo -en '200 OK\r\n{"status": "success", "message": "registered", "id": 1}' | sed '1d' | python3 -m json.tool) <(curl -s -X POST -H "Content-Type: application/json" -d '{"deadline": "2019-06-12T14:00:00+09:00", "title": "レポート提出", "memo": "foo"}' http://localhost:8080/api/v1/event | sed '1d' | python3 -m json.tool)
	@diff <(echo -en '200 OK\r\n{"status": "success", "message": "registered", "id": 2}' | sed '1d' | python3 -m json.tool) <(curl -s -X POST -H "Content-Type: application/json" -d '{"deadline": "2400-02-29T12:34:56Z", "title": "桜の樹の下には", "memo": "桜の樹の下には屍体したいが埋まっている！ 　これは信じていいことなんだよ。何故なぜって、桜の花があんなにも見事に咲くなんて信じられないことじゃないか。俺はあの美しさが信じられないので、この二三日不安だった。しかしいま、やっとわかるときが来た。桜の樹の下には屍体が埋まっている。これは信じていいことだ。 　どうして俺が毎晩家へ帰って来る道で、俺の部屋の数ある道具のうちの、選よりに選ってちっぽけな薄っぺらいもの、安全剃刀の刃なんぞが、千里眼のように思い浮かんで来るのか――おまえはそれがわからないと言ったが――そして俺にもやはりそれがわからないのだが――それもこれもやっぱり同じようなことにちがいない。 　いったいどんな樹の花でも、いわゆる真っ盛りという状態に達すると、あたりの空気のなかへ一種神秘な雰囲気を撒き散らすものだ。それは、よく廻った独楽こまが完全な静止に澄むように、また、音楽の上手な演奏がきまってなにかの幻覚を伴うように、灼熱しゃくねつした生殖の幻覚させる後光のようなものだ。それは人の心を撲うたずにはおかない、不思議な、生き生きとした、美しさだ。 　しかし、昨日、一昨日、俺の心をひどく陰気にしたものもそれなのだ。俺にはその美しさがなにか信じられないもののような気がした。俺は反対に不安になり、憂鬱ゆううつになり、空虚な気持になった。しかし、俺はいまやっとわかった。 　おまえ、この爛漫らんまんと咲き乱れている桜の樹の下へ、一つ一つ屍体が埋まっていると想像してみるがいい。何が俺をそんなに不安にしていたかがおまえには納得がいくだろう。 　馬のような屍体、犬猫のような屍体、そして人間のような屍体、屍体はみな腐爛ふらんして蛆うじが湧き、堪たまらなく臭い。それでいて水晶のような液をたらたらとたらしている。桜の根は貪婪どんらんな蛸たこのように、それを抱きかかえ、いそぎんちゃくの食糸のような毛根を聚あつめて、その液体を吸っている。 　何があんな花弁を作り、何があんな蕊しべを作っているのか、俺は毛根の吸いあげる水晶のような液が、静かな行列を作って、維管束のなかを夢のようにあがってゆくのが見えるようだ。 　――おまえは何をそう苦しそうな顔をしているのだ。美しい透視術じゃないか。俺はいまようやく瞳ひとみを据えて桜の花が見られるようになったのだ。昨日、一昨日、俺を不安がらせた神秘から自由になったのだ。 　二三日前、俺は、ここの溪たにへ下りて、石の上を伝い歩きしていた。水のしぶきのなかからは、あちらからもこちらからも、薄羽かげろうがアフロディットのように生まれて来て、溪の空をめがけて舞い上がってゆくのが見えた。おまえも知っているとおり、彼らはそこで美しい結婚をするのだ。しばらく歩いていると、俺は変なものに出喰でくわした。それは溪の水が乾いた磧かわらへ、小さい水溜を残している、その水のなかだった。思いがけない石油を流したような光彩が、一面に浮いているのだ。おまえはそれを何だったと思う。それは何万匹とも数の知れない、薄羽かげろうの屍体だったのだ。隙間なく水の面を被っている、彼らのかさなりあった翅はねが、光にちぢれて油のような光彩を流しているのだ。そこが、産卵を終わった彼らの墓場だったのだ。 　俺はそれを見たとき、胸が衝つかれるような気がした。墓場を発あばいて屍体を嗜このむ変質者のような残忍なよろこびを俺は味わった。 　この溪間ではなにも俺をよろこばすものはない。鶯うぐいすや四十雀しじゅうからも、白い日光をさ青に煙らせている木の若芽も、ただそれだけでは、もうろうとした心象に過ぎない。俺には惨劇が必要なんだ。その平衡があって、はじめて俺の心象は明確になって来る。俺の心は悪鬼のように憂鬱に渇いている。俺の心に憂鬱が完成するときにばかり、俺の心は和なごんでくる。 　――おまえは腋わきの下を拭ふいているね。冷汗が出るのか。それは俺も同じことだ。何もそれを不愉快がることはない。べたべたとまるで精液のようだと思ってごらん。それで俺達の憂鬱は完成するのだ。 　ああ、桜の樹の下には屍体が埋まっている！ 　いったいどこから浮かんで来た空想かさっぱり見当のつかない屍体が、いまはまるで桜の樹と一つになって、どんなに頭を振っても離れてゆこうとはしない。 　今こそ俺は、あの桜の樹の下で酒宴をひらいている村人たちと同じ権利で、花見の酒が呑のめそうな気がする。"}' http://localhost:8080/api/v1/event | tee out | sed '1d;2d' | python3 -m json.tool)
	#@diff <(echo -en '200 OK\r\n{"events":[]}' | sed '1d' | python3 -m json.tool) <(curl -s -X GET http://localhost:8080/api/v1/event | sed '1d' | python3 -m json.tool)
