'''
  根据商品条形码，从网络爬取其对应的商品名，及图片。 
'''

import os
import requests  
from lxml import etree


url_base = "http://www.1034.cn/search/?tm={}"

def SearchCommodityName(barcode):
    '''
       从 www.1034.cn 爬取二维码对应的商品名称 并返回
    '''
    final_url = url_base.format(barcode)
    res = requests.get(url = final_url)
    if res.status_code != 200:
        print("{} 爬取链接失败码：{}".format(barcode, res.status_code))
        return "未知商品 -404", -404       # http://www.1034.cn/search/?tm= 网络链接失败
    html_etree = etree.HTML(res.text)
    CommodityNameList = html_etree.xpath('//div[@class="panel-heading"]/h2/b')
    if CommodityNameList:
        CommodityName = CommodityNameList[0].text
        if CommodityName != "会员中心":
            return CommodityName, 0  # 爬取商品名成功
        else:
            return "未知商品 -100", -100   # 商品不存在 http://www.1034.cn/search/?tm=
    else:
        return "未知商品 -101", -101       # http://www.1034.cn/search/?tm= 页面变动

if __name__ == "__main__":

    barcode = "6937588928942"
    print(SearchCommodityName(barcode))


