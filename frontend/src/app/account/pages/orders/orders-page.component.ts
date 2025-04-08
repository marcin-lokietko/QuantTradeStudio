import { ChangeDetectorRef, Component } from '@angular/core';

@Component({
  selector: 'app-orders-page',
  standalone: false,
  templateUrl: './orders-page.component.html',
  styleUrls: ['./orders-page.component.scss']
})
export class OrdersPage {
  public orders = '';
  public isLoading = true;

  constructor(private cdr: ChangeDetectorRef) {}

  ngOnInit(): void {
    this.getData();
  }

  getData(): void {
    fetch('http://localhost:5000/openOrders')
      .then(response => {
        this.isLoading = false;
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.text();
      })
      .then(data => {
        try {
          this.orders = data;
          console.log(data)
          this.cdr.markForCheck();
        } catch (error) {
          console.error("Invalid orders content:", error);
        }
      })
      .catch(error => {
        console.error('Error fetching data:', error);
      });
  }
}
