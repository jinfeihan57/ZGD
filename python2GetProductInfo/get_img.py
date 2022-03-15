# 基本使用
from selenium import webdriver
from lxml import etree
import time
from contextlib import closing
import requests
from models import ProductInfo

# driver = webdriver.PhantomJS()

driver = webdriver.Chrome("C:\\Program Files (x86)\\python\\chromedriver.exe")

#driver = webdriver.Chrome("C:\\Program Files (x86)\\python3_8\\chromedriver.exe")

# 访问
info_URL = 'http://www.ancc.org.cn/Service/queryTools/internal.aspx'

img_path = './img/'

def DownLoadIMG(img_url, barcode, filename):
    
    with closing(requests.get(img_url)) as response: 
        if response.status_code == 200:
            with open(filename, 'wb') as f:
                f.write(response.content)
                f.close()
            return 0  # 下载图片成功
        else:
            return -1 # 链接错误
    return -1  # 链接错误

def GetImgAndallinfo(barcode):

    # 查询页面拟人操作
    driver.find_element_by_id('keyword').clear()
    driver.find_element_by_id('keyword').send_keys(barcode)
    driver.find_element_by_id('gdsBtn').click()

    # 数据库中获取对应的商品操作对象
    resbarinfo = ProductInfo.query.filter(ProductInfo.barcode == barcode).first()

    try:    # 判断商品是否搜索到
        result = driver.find_element_by_class_name('result')
    except:
        print(barcode + " not found")
        return '未知商品' # barcode Not found
    else:
        html_etree = etree.HTML(result.get_attribute('innerHTML'))

        productInfoList = html_etree.xpath('//a[@id="repList_ctl00_herl"]/@href')
        #for i in range(len(productInfoList)):
        print(productInfoList[0])

        filename = img_path + barcode + '.jpg'
        ret = DownLoadIMG(productInfoList[0], barcode, filename)    # 下载 商品图片
        if ret != 0:
            filename = None
        if filename:
            resbarinfo.img_path = filename
            resbarinfo.update()

        productInfoList = html_etree.xpath('//dl[@class="p-info"]/dd')
        
        resbarinfo.name = productInfoList[3].text
        resbarinfo.update()

        return productInfoList[3].text


# driver.find_element_by_id('keyword').clear()
# driver.find_element_by_id('keyword').send_keys('6920568400019')
# driver.find_element_by_id('gdsBtn').click()

# try:
#     result = driver.find_element_by_class_name('result')
# except:
#     print("6920568400019 NOT found")
# else:
#     print(result.text)
#     print(result.get_attribute('innerHTML'))

# driver.save_screenshot('6920568400019.png')

#barcodeList = ["6920568400019", "6923644223458"]

if __name__ == '__main__':

    driver.get(info_URL)
    time.sleep(30)      # 手动处理图片认证过程

    while True:
        # 每次读取是个条码进行信息爬取
        result = ProductInfo.query.filter(ProductInfo.img_path == None).limit(10).all()

        driver.refresh()
        
        for i in result:  # 循环获取所有的 商品条码对应的商品信息
            productName = GetImgAndallinfo(i.barcode)
            print(productName)
            time.sleep(3)

        if len(result) < 10:    # 最后一组条码处理完成跳出
            break

    driver.quit()