import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { AssetsPage } from './pages/assets/assets-page.component';
import { OrdersPage } from './pages/orders/orders-page.component';

const routes: Routes = [
  { path: 'assets', component: AssetsPage },
  { path: 'orders', component: OrdersPage },
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule]
})
export class AccountRoutingModule {}